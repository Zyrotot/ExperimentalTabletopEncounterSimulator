// -----------------------------------------------------------------------------
// | @file      damage_resolver.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/resolver/damage_resolver.h"

#include "internal/combat/damage_modification.h"
#include "internal/combat/event_manager.h"
#include "internal/entities/i_entity.h"  // IWYU pragma: keep
#include "internal/entities/stats.h"
#include "internal/logging/log_manager.h"
#include "internal/rules/resistances.h"

namespace ettes {
namespace resolver {

using entities::Resistances;

DamageResolver::DamageResolver(combat::CombatEventContext* context)
    : context_(context), logger_(logging::LogManager::GetLogger("attack")) {
}

DamageResolver::~DamageResolver() = default;

void DamageResolver::ApplySingleAttack(size_t result_index) {
  if (result_index >= context_->results.size()) {
    return;
  }

  auto& result = context_->results[result_index];
  context_->current_index = result_index;
  if (!result.is_hit) {
    return;
  }

  Resistances remaining_resistances = context_->target->GetResistances();

  for (auto& dmg_instance : result.damage_instances) {
    ApplyResistancesToDamage(&dmg_instance, &remaining_resistances);
  }

  auto modifications =
      combat::EmitCombatEvent(combat::CombatEvent::TakeDamage, context_);

  for (const auto& mod : modifications) {
    if (mod.negate_all) {
      for (auto& dmg : result.damage_instances) {
        dmg.amount = 0;
      }
    }
  }

  int total_damage = 0;
  for (const auto& dmg_instance : result.damage_instances) {
    total_damage += dmg_instance.amount;
  }

  combat::EmitCombatEvent(combat::CombatEvent::DealDamage, context_);

  if (total_damage > 0) {
    logger_->info("Total damage applied: {}", total_damage);

    context_->target->TakeDamage(total_damage);
    bool is_alive = context_->target->IsAlive();

    if (!is_alive) {
      logger_->info("{} killed {}!", context_->source->GetName(),
                    context_->target->GetName());
      combat::EmitCombatEvent(combat::CombatEvent::Kill, context_);
    }
  }
}

void DamageResolver::ApplyResistancesToDamage(
    rules::DamageInstance* dmg_instance, Resistances* resistances) {
  auto logger = logging::LogManager::GetLogger("attack");

  if ((dmg_instance->types &
       static_cast<uint16_t>(resistances->immunity.immune_types)) != 0) {
    logger->debug("Damage fully negated by immunity");
    dmg_instance->amount = 0;
    return;
  }

  for (auto& weakness : resistances->weaknesses) {
    if ((dmg_instance->types & static_cast<uint16_t>(weakness.weak_types)) !=
        0) {
      logger->debug("Damage increased by weakness ({} -> {})",
                    dmg_instance->amount,
                    dmg_instance->amount + weakness.amount);
      dmg_instance->amount += weakness.amount;
    }
  }

  if (GetDamageCategory(static_cast<rules::DamageType>(dmg_instance->types)) ==
      rules::DamageCategory::Physical) {
    for (auto& dr : resistances->damage_reductions) {
      if (dr.amount <= 0) {
        continue;
      }
      if (((dmg_instance->types & static_cast<uint16_t>(dr.bypass_types)) ==
           0) ||
          ((dmg_instance->modifiers &
            static_cast<uint16_t>(dr.bypass_modifiers)) == 0)) {
        if (dmg_instance->amount > dr.amount) {
          logger->debug("Damage reduction of {} applied", dr.amount);
          dmg_instance->amount -= dr.amount;
          dr.amount = 0;
        } else {
          logger->debug(
              "Damage reduction of {} applied, but damage fully absorbed",
              dmg_instance->amount);
          dr.amount -= dmg_instance->amount;
          dmg_instance->amount = 0;
        }
      } else {
        logger->debug("Damage reduction of {} bypassed by modifiers/types",
                      dr.amount);
      }
    }
  } else if (GetDamageCategory(static_cast<rules::DamageType>(
                 dmg_instance->types)) == rules::DamageCategory::Energy) {
    for (auto& er : resistances->energy_resistances) {
      if ((dmg_instance->types & static_cast<uint16_t>(er.resistance_type)) !=
          0) {
        if (dmg_instance->amount > er.amount) {
          logger->debug("Energy resistance of {} applied", er.amount);
          dmg_instance->amount -= er.amount;
          er.amount = 0;
        } else {
          logger->debug(
              "Energy resistance of {} applied, but damage fully absorbed",
              dmg_instance->amount);
          er.amount -= dmg_instance->amount;
          dmg_instance->amount = 0;
        }
      }
    }
  }

  if (dmg_instance->amount < 0) {
    dmg_instance->amount = 0;
  }
}

}  // namespace resolver
}  // namespace ettes

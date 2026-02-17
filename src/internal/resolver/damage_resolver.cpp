// -----------------------------------------------------------------------------
// | @file      damage_resolver.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/resolver/damage_resolver.h"
#include "internal/combat/event_manager.h"
#include "internal/entities/entity.h" // IWYU pragma: keep
#include "internal/entities/stats.h"
#include "internal/logging/log_manager.h"
#include "internal/rules/resistances.h"

namespace internal {
namespace resolver {

using combat::CombatContext;
using entities::Resistances;

DamageResolver::DamageResolver(std::shared_ptr<CombatContext> context)
    : context_(context), logger_(logging::LogManager::GetLogger("attack")) {}

DamageResolver::~DamageResolver() {}

void DamageResolver::ApplyDamage() {
  for (auto &result : context_->results) {
    if (!result.is_hit) {
      continue;
    }

    auto damage_context = std::make_shared<combat::DamageContext>(
        context_->attacker, context_->target);
    damage_context->damage = result.damage_instances;

    Resistances remaining_resistances = context_->target->GetResistances();

    for (auto &dmg_instance : damage_context->damage) {
      ApplyResistancesToDamage(&dmg_instance, &remaining_resistances);
    }

    combat::EventManager::Emit(combat::CombatEvent::TakeDamage, damage_context);

    int total_damage = 0;
    for (const auto &dmg_instance : damage_context->damage) {
      total_damage += dmg_instance.amount;
    }

    combat::EventManager::Emit(combat::CombatEvent::DealDamage, damage_context);

    if (total_damage > 0) {
      logger_->Info("Total damage applied: {}", total_damage);

      context_->target->TakeDamage(total_damage);
      bool is_alive = context_->target->IsAlive();

      if (!is_alive) {
        logger_->Info("{} killed {}!", context_->attacker->GetName(),
                      context_->target->GetName());
        combat::EventManager::Emit(combat::CombatEvent::Kill, damage_context);
      }
    }
  }
}

void DamageResolver::ApplyResistancesToDamage(
    rules::DamageInstance *dmg_instance, Resistances *resistances) {
  auto logger = logging::LogManager::GetLogger("attack");

  if ((dmg_instance->types &
       static_cast<uint16_t>(resistances->immunity.immune_types)) != 0) {
    logger->Debug("Damage fully negated by immunity");
    dmg_instance->amount = 0;
    return;
  }

  for (auto &weakness : resistances->weaknesses) {
    if ((dmg_instance->types & static_cast<uint16_t>(weakness.weak_types)) !=
        0) {
      logger->Debug("Damage increased by weakness ({} -> {})",
                    dmg_instance->amount,
                    dmg_instance->amount + weakness.amount);
      dmg_instance->amount += weakness.amount;
    }
  }

  if (GetDamageCategory(static_cast<rules::DamageType>(dmg_instance->types)) ==
      rules::DamageCategory::Physical) {
    for (auto &dr : resistances->damage_reductions) {
      if (dr.amount <= 0) {
        continue;
      }
      if (((dmg_instance->types & static_cast<uint16_t>(dr.bypass_types)) ==
           0) ||
          ((dmg_instance->modifiers &
            static_cast<uint16_t>(dr.bypass_modifiers)) == 0)) {
        if (dmg_instance->amount > dr.amount) {
          logger->Debug("Damage reduction of {} applied", dr.amount);
          dmg_instance->amount -= dr.amount;
          dr.amount = 0;
        } else {
          logger->Debug(
              "Damage reduction of {} applied, but damage fully absorbed",
              dmg_instance->amount);
          dr.amount -= dmg_instance->amount;
          dmg_instance->amount = 0;
        }
      } else {
        logger->Debug("Damage reduction of {} bypassed by modifiers/types",
                      dr.amount);
      }
    }
  } else if (GetDamageCategory(static_cast<rules::DamageType>(
                 dmg_instance->types)) == rules::DamageCategory::Energy) {
    for (auto &er : resistances->energy_resistances) {
      if ((dmg_instance->types & static_cast<uint16_t>(er.resistance_type)) !=
          0) {
        if (dmg_instance->amount > er.amount) {
          logger->Debug("Energy resistance of {} applied", er.amount);
          dmg_instance->amount -= er.amount;
          er.amount = 0;
        } else {
          logger->Debug(
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

} // namespace resolver
} // namespace internal
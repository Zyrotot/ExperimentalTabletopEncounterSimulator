// -----------------------------------------------------------------------------
// | @file      damage_resolver.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/resolver/damage_resolver.h"
#include "internal/entities/entity.h"  // IWYU pragma: keep
#include "internal/entities/stats.h"
#include "internal/rules/resistances.h"

namespace internal {
namespace resolver {

using combat::CombatContext;
using entities::Resistances;

DamageResolver::DamageResolver(std::shared_ptr<CombatContext> context)
    : context_(context) {}

DamageResolver::~DamageResolver() {}

void DamageResolver::ApplyDamage() {
  for (auto &result : context_->results) {
    Resistances remaining_resistances = context_->target->GetResistancesCopy();
    if (!result.is_hit) {
      continue;
    }
    int total_damage = 0;
    for (auto &dmg_instance : result.damage_instances) {
      ApplyResistances(&dmg_instance, &remaining_resistances);
      total_damage += dmg_instance.amount;
    }
    context_->target->TakeDamage(total_damage);

    if (result.attack && result.attack->weapon) {
      for (const auto &enchantment : result.attack->weapon->enchantments) {
        for (const auto &effect : enchantment.effects) {
          if (effect.trigger == combat::CombatEvent::DealDamage) {
            effect.on(context_);
          }
        }
      }
    }
  }
}

void DamageResolver::ApplyResistances(rules::DamageInstance *dmg_instance,
                                      Resistances *resistances) {
  if ((dmg_instance->types &
       static_cast<uint16_t>(resistances->immunity.immune_types)) != 0) {
    dmg_instance->amount = 0;
    return;
  }

  for (auto &weakness : resistances->weaknesses) {
    if ((dmg_instance->types & static_cast<uint16_t>(weakness.weak_types)) !=
        0) {
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
          dmg_instance->amount -= dr.amount;
          dr.amount = 0;
        } else {
          dr.amount -= dmg_instance->amount;
          dmg_instance->amount = 0;
        }
      }
    }
  } else if (GetDamageCategory(static_cast<rules::DamageType>(
                 dmg_instance->types)) == rules::DamageCategory::Energy) {
    for (auto &er : resistances->energy_resistances) {
      if ((dmg_instance->types & static_cast<uint16_t>(er.resistance_type)) !=
          0) {
        if (dmg_instance->amount > er.amount) {
          dmg_instance->amount -= er.amount;
          er.amount = 0;
        } else {
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
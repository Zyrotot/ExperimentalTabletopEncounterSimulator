// -----------------------------------------------------------------------------
// | @file      ability.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/abilities/ability.h"

#include "internal/entities/entity.h"  // IWYU pragma: keep
#include "internal/entities/stats.h"
#include "internal/rules/resistances.h"

namespace internal {
namespace abilities {

Ability CreateErosao() {
  Ability ability;
  ability.name = "Erosion";
  ability.is_active = true;

  combat::Effect effect;
  effect.name = "Erosion";
  effect.source = "Ability: Erosion";
  effect.trigger = combat::CombatEvent::Hit;
  effect.on_event = [](std::shared_ptr<combat::CombatEventContext> context) {
    if (!context || !context->target)
      return;
    auto& bonus_dr = context->target->GetCurrentStats()
                         .bonus_stats.bonus_resistances.damage_reductions;
    if (!bonus_dr.empty()) {
      context->target->RemoveDR(1, true);
    } else {
      context->target->RemoveDR(1, false);
    }
  };

  ability.effects.push_back(effect);
  return ability;
}

Ability CreateRigidezRaivosa() {
  Ability ability;
  ability.name = "Rigidez Raivosa";
  ability.is_active = true;
  ability.stack_count = 0;

  combat::Effect take_damage_effect;
  take_damage_effect.name = "Rigidez Raivosa";
  take_damage_effect.source = "Ability: Rigidez Raivosa";
  take_damage_effect.trigger = combat::CombatEvent::TakeDamage;
  take_damage_effect.on_event =
      [](std::shared_ptr<combat::CombatEventContext> context) {
        if (!context || !context->target)
          return;

        rules::DamageReduction dr;
        dr.amount = 1;
        dr.bypass_types = rules::DamageType::None;
        context->target->AddDR(dr, true);
        context->target->IncrementAbilityStack("Rigidez Raivosa");
      };

  combat::Effect heal_effect;
  heal_effect.name = "Rigidez Raivosa";
  heal_effect.source = "Ability: Rigidez Raivosa";
  heal_effect.trigger = combat::CombatEvent::Heal;
  heal_effect.on_event =
      [](std::shared_ptr<combat::CombatEventContext> context) {
        if (!context || !context->target || context->is_temp_hp)
          return;

        context->target->ClearAllDR(true);
        context->target->SetAbilityStack("Rigidez Raivosa", 0);
      };

  ability.effects.push_back(take_damage_effect);
  ability.effects.push_back(heal_effect);
  return ability;
}

Ability CreateTrespassar() {
  Ability ability;
  ability.name = "Trespassar";
  ability.is_active = true;

  combat::Effect effect;
  effect.name = "Trespassar";
  effect.source = "Ability: Trespassar";
  effect.trigger = combat::CombatEvent::Kill;
  effect.on_event = [](std::shared_ptr<combat::CombatEventContext> context) {
    if (!context || !context->source || !context->target)
      return;
    if (!context->target->IsAlive()) {
      context->source->IncrementAbilityStack("Trespassar");
      // TODO(zyrotot): Trigger additional attack here - need to add attack
      // queue system
    }
  };

  ability.effects.push_back(effect);
  return ability;
}

Ability CreateDuroDeFerir(int stacks) {
  Ability ability;
  ability.name = "Duro de Ferir";
  ability.is_active = true;
  ability.stack_count = stacks;

  combat::Effect effect;
  effect.name = "Duro de Ferir";
  effect.source = "Ability: Duro de Ferir";
  effect.trigger = combat::CombatEvent::TakeDamage;
  effect.on_event = [](std::shared_ptr<combat::CombatEventContext> context) {
    if (!context || !context->target) {
      return;
    }

    if (context->current_index >= context->results.size()) {
      return;
    }

    auto current_result = context->results[context->current_index];

    if (context->target->GetAbilityStack("Duro de Ferir") > 0) {
      for (auto& dmg : current_result.damage_instances) {
        dmg.amount = 0;
      }
      context->target->DecrementAbilityStack("Duro de Ferir");
    }
  };

  ability.effects.push_back(effect);
  return ability;
}

Ability CreateDuroDeMatar(int stacks) {
  Ability ability;
  ability.name = "Duro de Matar";
  ability.is_active = true;
  ability.stack_count = stacks;

  combat::Effect effect;
  effect.name = "Duro de Matar";
  effect.source = "Ability: Duro de Matar";
  effect.trigger = combat::CombatEvent::TakeDamage;
  effect.on_event = [](std::shared_ptr<combat::CombatEventContext> context) {
    if (!context || !context->target) {
      return;
    }

    if (context->current_index >= context->results.size()) {
      return;
    }

    auto current_result = context->results[context->current_index];

    if (context->target->GetAbilityStack("Duro de Matar") > 0) {
      int total_damage = 0;

      for (const auto& dmg : current_result.damage_instances) {
        total_damage += dmg.amount;
      }

      int current_hp =
          context->target->GetCurrentStats().base_stats.hp +
          context->target->GetCurrentStats().bonus_stats.temporary_hp;

      if (total_damage >= current_hp) {
        for (auto& dmg : current_result.damage_instances) {
          dmg.amount = 0;
        }
        context->target->DecrementAbilityStack("Duro de Matar");
      }
    }
  };

  ability.effects.push_back(effect);
  return ability;
}

}  // namespace abilities
}  // namespace internal

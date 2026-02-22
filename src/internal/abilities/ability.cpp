// -----------------------------------------------------------------------------
// | @file      ability.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/abilities/ability.h"

#include "internal/combat/combat_context.h"
#include "internal/combat/damage_modification.h"
#include "internal/entities/i_entity.h"  // IWYU pragma: keep
#include "internal/rules/resistances.h"

namespace ettes {
namespace abilities {

Ability CreateErosao() {
  Ability ability;
  ability.name = "Erosion";
  ability.id = AbilityId::Erosao;
  ability.is_active = true;

  combat::Effect effect;
  effect.name = "Erosion";
  effect.source = "Ability: Erosion";
  effect.trigger = combat::CombatEvent::Hit;
  effect.on_event = [](const combat::CombatEventContext& context) {
    if (!context.target)
      return;
    auto& bonus_dr = context.target->GetCurrentStats()
                         .bonus_stats.bonus_resistances.damage_reductions;
    if (!bonus_dr.empty()) {
      context.target->RemoveDR(1, true);
    } else {
      context.target->RemoveDR(1, false);
    }
  };

  ability.effects.push_back(effect);
  return ability;
}

Ability CreateRigidezRaivosa() {
  Ability ability;
  ability.name = "Rigidez Raivosa";
  ability.id = AbilityId::RigidezRaivosa;
  ability.is_active = true;
  ability.stack_count = 0;

  combat::Effect take_damage_effect;
  take_damage_effect.name = "Rigidez Raivosa";
  take_damage_effect.source = "Ability: Rigidez Raivosa";
  take_damage_effect.trigger = combat::CombatEvent::TakeDamage;
  take_damage_effect.on_event = [](const combat::CombatEventContext& context) {
    if (!context.target)
      return;

    rules::DamageReduction dr;
    dr.amount = 1;
    dr.bypass_types = rules::DamageType::None;
    context.target->AddDR(dr, true);
    context.target->IncrementAbilityStack(AbilityId::RigidezRaivosa);
  };

  combat::Effect heal_effect;
  heal_effect.name = "Rigidez Raivosa";
  heal_effect.source = "Ability: Rigidez Raivosa";
  heal_effect.trigger = combat::CombatEvent::Heal;
  heal_effect.on_event = [](const combat::CombatEventContext& context) {
    if (!context.target || context.is_temp_hp)
      return;

    context.target->ClearAllDR(true);
    context.target->SetAbilityStack(AbilityId::RigidezRaivosa, 0);
  };

  ability.effects.push_back(take_damage_effect);
  ability.effects.push_back(heal_effect);
  return ability;
}

Ability CreateTrespassar() {
  Ability ability;
  ability.name = "Trespassar";
  ability.id = AbilityId::Trespassar;
  ability.is_active = true;
  ability.stack_count = 1;

  combat::Effect effect;
  effect.name = "Trespassar";
  effect.source = "Ability: Trespassar";
  effect.trigger = combat::CombatEvent::Kill;
  effect.on_event = [](const combat::CombatEventContext& context) {
    if (!context.source) {
      return;
    }
    if (context.source->GetAbilityStack(AbilityId::Trespassar) > 0) {
      context.source->DecrementAbilityStack(AbilityId::Trespassar);
      if (context.attack_queue) {
        context.attack_queue->QueueAttack({context.source, nullptr, 0});
      }
    }
  };

  ability.effects.push_back(effect);

  combat::Effect recharge_effect;
  recharge_effect.name = "Trespassar";
  recharge_effect.source = "Recharge: Trespassar";
  recharge_effect.trigger = combat::CombatEvent::TurnStart;
  recharge_effect.on_event = [](const combat::CombatEventContext& context) {
    if (!context.source) {
      return;
    }
    context.source->SetAbilityStack(AbilityId::Trespassar, 1);
  };

  ability.effects.push_back(recharge_effect);

  return ability;
}

Ability CreateDuroDeFerir(int stacks) {
  Ability ability;
  ability.name = "Duro de Ferir";
  ability.id = AbilityId::DuroDeFerir;
  ability.is_active = true;
  ability.stack_count = stacks;

  combat::Effect effect;
  effect.name = "Duro de Ferir";
  effect.source = "Ability: Duro de Ferir";
  effect.trigger = combat::CombatEvent::TakeDamage;
  effect.on_damage = [](const combat::CombatEventContext& context)
      -> combat::DamageModification {
    if (!context.target) {
      return {};
    }

    if (context.target->GetAbilityStack(AbilityId::DuroDeFerir) > 0) {
      context.target->DecrementAbilityStack(AbilityId::DuroDeFerir);
      return {.negate_all = true};
    }
    return {};
  };

  ability.effects.push_back(effect);
  return ability;
}

Ability CreateDuroDeMatar(int stacks) {
  Ability ability;
  ability.name = "Duro de Matar";
  ability.id = AbilityId::DuroDeMatar;
  ability.is_active = true;
  ability.stack_count = stacks;

  combat::Effect effect;
  effect.name = "Duro de Matar";
  effect.source = "Ability: Duro de Matar";
  effect.trigger = combat::CombatEvent::TakeDamage;
  effect.on_damage = [](const combat::CombatEventContext& context)
      -> combat::DamageModification {
    if (!context.target) {
      return {};
    }

    if (context.current_index >= context.results.size()) {
      return {};
    }

    const auto& current_result = context.results[context.current_index];

    if (context.target->GetAbilityStack(AbilityId::DuroDeMatar) > 0) {
      int total_damage = 0;

      for (const auto& dmg : current_result.damage_instances) {
        total_damage += dmg.amount;
      }

      int current_hp =
          context.target->GetCurrentStats().base_stats.hp +
          context.target->GetCurrentStats().bonus_stats.temporary_hp;

      if (total_damage >= current_hp) {
        context.target->DecrementAbilityStack(AbilityId::DuroDeMatar);
        return {.negate_all = true};
      }
    }
    return {};
  };

  ability.effects.push_back(effect);
  return ability;
}

}  // namespace abilities
}  // namespace ettes

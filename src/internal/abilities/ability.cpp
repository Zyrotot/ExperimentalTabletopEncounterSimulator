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

  AbilityEffect effect;
  effect.trigger = combat::CombatEvent::Hit;
  effect.on_event = [](std::shared_ptr<combat::EventContext> base_context) {
    auto context = std::dynamic_pointer_cast<combat::HitContext>(base_context);
    if (!context || !context->target)
      return;
    // TODO(zyrotot): Fix coredump
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

  AbilityEffect take_damage_effect;
  take_damage_effect.trigger = combat::CombatEvent::TakeDamage;
  take_damage_effect.on_event =
      [](std::shared_ptr<combat::EventContext> base_context) {
        auto context =
            std::dynamic_pointer_cast<combat::DamageContext>(base_context);
        if (!context || !context->target)
          return;

        rules::DamageReduction dr;
        dr.amount = 1;
        dr.bypass_types = rules::DamageType::None;
        context->target->AddDR(dr, true);
        context->target->IncrementAbilityStack("Rigidez Raivosa");
      };

  AbilityEffect heal_effect;
  heal_effect.trigger = combat::CombatEvent::Heal;
  heal_effect.on_event =
      [](std::shared_ptr<combat::EventContext> base_context) {
        auto context =
            std::dynamic_pointer_cast<combat::HealContext>(base_context);
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

  AbilityEffect effect;
  effect.trigger = combat::CombatEvent::Kill;
  effect.on_event = [](std::shared_ptr<combat::EventContext> base_context) {
    auto context =
        std::dynamic_pointer_cast<combat::DamageContext>(base_context);
    if (!context || !context->attacker || !context->target)
      return;
    if (!context->target->IsAlive()) {
      context->attacker->IncrementAbilityStack("Trespassar");
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

  AbilityEffect effect;
  effect.trigger = combat::CombatEvent::TakeDamage;
  effect.on_event = [](std::shared_ptr<combat::EventContext> base_context) {
    auto context =
        std::dynamic_pointer_cast<combat::DamageContext>(base_context);
    if (!context || !context->target)
      return;

    if (context->target->GetAbilityStack("Duro de Ferir") > 0) {
      for (auto& dmg : context->damage) {
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

  AbilityEffect effect;
  effect.trigger = combat::CombatEvent::TakeDamage;
  effect.on_event = [](std::shared_ptr<combat::EventContext> base_context) {
    auto context =
        std::dynamic_pointer_cast<combat::DamageContext>(base_context);
    if (!context || !context->target)
      return;

    if (context->target->GetAbilityStack("Duro de Matar") > 0) {
      int total_damage = 0;
      for (const auto& dmg : context->damage) {
        total_damage += dmg.amount;
      }

      int current_hp =
          context->target->GetCurrentStats().base_stats.hp +
          context->target->GetCurrentStats().bonus_stats.temporary_hp;

      if (total_damage >= current_hp) {
        for (auto& dmg : context->damage) {
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

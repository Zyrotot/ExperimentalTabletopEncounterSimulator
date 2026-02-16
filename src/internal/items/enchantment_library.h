// -----------------------------------------------------------------------------
// | @file      enchantment_library.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ITEMS_ENCHANTMENT_LIBRARY_H_
#define SRC_INTERNAL_ITEMS_ENCHANTMENT_LIBRARY_H_

#include <algorithm>
#include <cstdint>

#include "internal/combat/combat_events.h"
#include "internal/dice_rolls/roller.h"
#include "internal/entities/entity.h"
#include "internal/items/enchantment.h"
#include "internal/rules/alignment.h"
#include "internal/rules/damage_types.h"

namespace internal {
namespace items {

using combat::CombatEvent;
using dice_rolls::Roller;
using dice_rolls::Term;
using entities::Entity;
using rules::DamageInstance;
using rules::DamageModifier;
using rules::DamageType;

inline Enchantment CreateFlamingEnchantment() {
  Enchantment ench;
  ench.name = "FlamingWeapon";

  DamageSource source;
  source.name = "Flaming";
  source.contribute = [](const combat::CombatContext& /* unused */, Roller& roller) {
    return DamageInstance{.amount = roller.Roll(Term{.dice_groups = {{1, 6}}}),
                          .types = static_cast<uint16_t>(DamageType::Fire),
                          .modifiers = 0};
  };
  ench.damage_sources.push_back(source);

  return ench;
}

inline Enchantment CreateDissonantEnchantment() {
  Enchantment ench;
  ench.name = "Dissonant";

  DamageSource source;
  source.name = "Dissonant";
  source.contribute = [](const combat::CombatContext& /* unused */, Roller& roller) {
    return DamageInstance{.amount = roller.Roll(Term{.dice_groups = {{2, 6}}}),
                          .types = static_cast<uint16_t>(DamageType::Negative),
                          .modifiers = 0};
  };
  ench.damage_sources.push_back(source);

  Effect effect;
  effect.name = "Dissonant (self-damage)";
  effect.trigger = CombatEvent::DealDamage;
  effect.on = [](const combat::CombatContext& ctx) {
    int self_damage = ctx.roller->Roll(Term{.dice_groups = {{1, 6}}});
    ctx.attacker->TakeDamage(self_damage);  // TODO(zyrotot): Resolve trough damage dealer to apply resistances, etc.
  };
  ench.effects.push_back(effect);

  return ench;
}

inline Enchantment CreateFlamingExplosionEnchantment() {
  Enchantment ench;
  ench.name = "Flaming Explosion";

  DamageSource source;
  source.name = "Flaming Explosion";
  source.contribute = [](const combat::CombatContext& ctx, Roller& roller) {
    int damage;
    if (ctx.is_crit) {
      int dice = std::min(ctx.crit_multiplier - 1, 3);
      damage = roller.Roll(Term{.dice_groups = {{dice, 10}, {1, 6}}});
    } else {
      damage = roller.Roll(Term{.dice_groups = {{1, 6}}});
    }
    return DamageInstance{
        .amount = damage, .types = static_cast<uint16_t>(DamageType::Fire), .modifiers = 0};
  };
  ench.damage_sources.push_back(source);

  return ench;
}

inline Enchantment CreateVampiricEnchantment() {
  Enchantment ench;
  ench.name = "Vampiric";

  Effect effect;
  effect.name = "Vampiric";
  effect.trigger = CombatEvent::DealDamage;
  effect.on = [](const combat::CombatContext& ctx) {
    if (ctx.damage_instances.empty()) {
      return;
    }
    int total_damage = 0;
    for (const auto& dmg : ctx.damage_instances) {
      total_damage += dmg.amount;
    }

    int temp_hp = total_damage / 2;
    ctx.attacker->AddTempHP(temp_hp);
  };
  ench.effects.push_back(effect);

  return ench;
}

inline Enchantment CreateDrainingEnchantment() {
  Enchantment ench;
  ench.name = "Draining";

  Effect effect;
  effect.name = "Draining";
  effect.trigger = CombatEvent::Hit;
  effect.on = [](const combat::CombatContext& ctx) {
    ctx.attacker->Heal(1);
  };
  ench.effects.push_back(effect);

  return ench;
}

inline Enchantment CreateProfaneEnchantment() {
  Enchantment ench;
  ench.name = "Profane";

  DamageSource source;
  source.name = "Profane";
  source.contribute = [](const combat::CombatContext& ctx, Roller& roller) {
    if (static_cast<uint16_t>(ctx.target->GetAlignment()) &
        static_cast<uint16_t>(rules::Alignment::Good)) {
      return DamageInstance{
          .amount = roller.Roll(Term{.dice_groups = {{2, 6}}}),
          .types = 0,
          .modifiers = static_cast<uint16_t>(DamageModifier::Evil)};
    }
    return DamageInstance{};
  };
  ench.damage_sources.push_back(source);

  return ench;
}

}  // namespace items
}  // namespace internal

#endif  // SRC_INTERNAL_ITEMS_ENCHANTMENT_LIBRARY_H_

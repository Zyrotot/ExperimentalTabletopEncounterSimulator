// -----------------------------------------------------------------------------
// | @file      enchantment_library.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/items/enchantment_library.h"

#include <algorithm>
#include <cstdint>
#include <memory>

#include "internal/combat/combat_context.h"
#include "internal/dice_rolls/roller.h"
#include "internal/entities/i_entity.h"  // IWYU pragma: keep
#include "internal/items/enchantment.h"
#include "internal/resolver/damage_resolver.h"
#include "internal/rules/alignment.h"
#include "internal/rules/damage_types.h"

namespace internal {
namespace items {

Enchantment CreateFlamingEnchantment() {
  Enchantment ench;
  ench.name = "FlamingWeapon";

  DamageSource source;
  source.name = "Flaming";
  source.contribute = [](const combat::CombatEventContext& ctx) {
    return rules::DamageInstance{
        .amount = ctx.roller->Roll(dice_rolls::Term{.dice_groups = {{1, 6}}}),
        .types = static_cast<uint16_t>(rules::DamageType::Fire),
        .modifiers = 0};
  };
  ench.damage_sources.push_back(source);

  return ench;
}

Enchantment CreateDissonantEnchantment() {
  Enchantment ench;
  ench.name = "Dissonant";

  DamageSource source;
  source.name = "Dissonant";
  source.contribute = [](const combat::CombatEventContext& ctx) {
    return rules::DamageInstance{
        .amount = ctx.roller->Roll(dice_rolls::Term{.dice_groups = {{2, 6}}}),
        .types = static_cast<uint16_t>(rules::DamageType::Negative),
        .modifiers = 0};
  };
  ench.damage_sources.push_back(source);

  combat::Effect effect;
  effect.name = "Dissonant (self-damage)";
  effect.source = "Weapon Enchantment: Dissonant";
  effect.trigger = combat::CombatEvent::Hit;
  effect.on_event = [](const combat::CombatEventContext& ctx) {
    if (!ctx.source || !ctx.roller)
      return;

    rules::DamageInstance self_damage = {
        .amount = ctx.roller->Roll(dice_rolls::Term{.dice_groups = {{1, 6}}}),
        .types = static_cast<uint16_t>(rules::DamageType::Negative),
        .modifiers = 0};
    entities::Resistances target_resistances = ctx.source->GetResistances();
    resolver::DamageResolver::ApplyResistancesToDamage(&self_damage,
                                                       &target_resistances);
    ctx.source->TakeDamage(self_damage.amount);
  };
  ench.effects.push_back(effect);

  return ench;
}

Enchantment CreateFlamingExplosionEnchantment() {
  Enchantment ench;
  ench.name = "Flaming Explosion";

  DamageSource source;
  source.name = "Flaming Explosion";
  source.contribute = [](const combat::CombatEventContext& ctx) {
    if (ctx.results.empty()) {
      return rules::DamageInstance{};
    }

    const auto& result = ctx.results.back();
    int damage;
    if (result.is_crit) {
      int dice = std::min(result.crit_multiplier - 1, 3);
      damage = ctx.roller->Roll(
          dice_rolls::Term{.dice_groups = {{dice, 10}, {1, 6}}});
    } else {
      damage = ctx.roller->Roll(dice_rolls::Term{.dice_groups = {{1, 6}}});
    }
    return rules::DamageInstance{
        .amount = damage,
        .types = static_cast<uint16_t>(rules::DamageType::Fire),
        .modifiers = 0};
  };
  ench.damage_sources.push_back(source);

  return ench;
}

Enchantment CreateVampiricEnchantment() {
  Enchantment ench;
  ench.name = "Vampiric";

  combat::Effect effect;
  effect.name = "Vampiric";
  effect.source = "Weapon Enchantment: Vampiric";
  effect.trigger = combat::CombatEvent::DealDamage;
  effect.on_event = [](const combat::CombatEventContext& ctx) {
    if (ctx.current_index >= ctx.results.size()) {
      return;
    }
    const auto current_result = ctx.results[ctx.current_index];

    int total_damage = 0;
    for (const auto& dmg : current_result.damage_instances) {
      total_damage += dmg.amount;
    }

    int temp_hp = total_damage / 2;
    if (ctx.source) {
      ctx.source->AddTempHP(temp_hp);
    }
  };
  ench.effects.push_back(effect);

  return ench;
}

Enchantment CreateDrainingEnchantment() {
  Enchantment ench;
  ench.name = "Draining";

  combat::Effect effect;
  effect.name = "Draining";
  effect.source = "Weapon Enchantment: Draining";
  effect.trigger = combat::CombatEvent::Hit;
  effect.on_event = [](const combat::CombatEventContext& ctx) {
    if (!ctx.source)
      return;

    ctx.source->Heal(1);
  };
  ench.effects.push_back(effect);

  return ench;
}

Enchantment CreateProfaneEnchantment() {
  Enchantment ench;
  ench.name = "Profane";

  DamageSource source;
  source.name = "Profane";
  source.contribute = [](const combat::CombatEventContext& ctx) {
    if (!ctx.target)
      return rules::DamageInstance{};

    if (static_cast<uint16_t>(ctx.target->GetAlignment()) &
        static_cast<uint16_t>(rules::Alignment::Good)) {
      return rules::DamageInstance{
          .amount = ctx.roller->Roll(dice_rolls::Term{.dice_groups = {{2, 6}}}),
          .types = 0,
          .modifiers = static_cast<uint16_t>(rules::DamageModifier::Evil)};
    }
    return rules::DamageInstance{};
  };
  ench.damage_sources.push_back(source);

  return ench;
}

}  // namespace items
}  // namespace internal

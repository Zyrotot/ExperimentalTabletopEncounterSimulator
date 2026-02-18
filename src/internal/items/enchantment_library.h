// -----------------------------------------------------------------------------
// | @file      enchantment_library.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ITEMS_ENCHANTMENT_LIBRARY_H_
#define SRC_INTERNAL_ITEMS_ENCHANTMENT_LIBRARY_H_

#include <algorithm>
#include <cstdint>
#include <memory>

#include "internal/combat/combat_events.h"
#include "internal/dice_rolls/roller.h"
#include "internal/entities/entity.h"  // IWYU pragma: keep
#include "internal/items/enchantment.h"
#include "internal/resolver/damage_resolver.h"
#include "internal/rules/alignment.h"
#include "internal/rules/damage_types.h"

namespace internal {
namespace items {

inline Enchantment CreateFlamingEnchantment() {
  Enchantment ench;
  ench.name = "FlamingWeapon";

  DamageSource source;
  source.name = "Flaming";
  source.contribute = [](std::shared_ptr<combat::CombatEventContext> ctx) {
    if (!ctx)
      return rules::DamageInstance{};

    return rules::DamageInstance{
        .amount = ctx->roller->Roll(dice_rolls::Term{.dice_groups = {{1, 6}}}),
        .types = static_cast<uint16_t>(rules::DamageType::Fire),
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
  source.contribute = [](std::shared_ptr<combat::CombatEventContext> ctx) {
    if (!ctx)
      return rules::DamageInstance{};

    return rules::DamageInstance{
        .amount = ctx->roller->Roll(dice_rolls::Term{.dice_groups = {{2, 6}}}),
        .types = static_cast<uint16_t>(rules::DamageType::Negative),
        .modifiers = 0};
  };
  ench.damage_sources.push_back(source);

  combat::Effect effect;
  effect.name = "Dissonant (self-damage)";
  effect.source = "Weapon Enchantment: Dissonant";
  effect.trigger = combat::CombatEvent::Hit;
  effect.on_event = [](std::shared_ptr<combat::CombatEventContext> ctx) {
    if (!ctx || !ctx->source || !ctx->roller)
      return;

    rules::DamageInstance self_damage = {
        .amount = ctx->roller->Roll(dice_rolls::Term{.dice_groups = {{1, 6}}}),
        .types = static_cast<uint16_t>(rules::DamageType::Negative),
        .modifiers = 0};
    entities::Resistances target_resistances = ctx->source->GetResistances();
    resolver::DamageResolver::ApplyResistancesToDamage(&self_damage,
                                                       &target_resistances);
    ctx->source->TakeDamage(self_damage.amount);
  };
  ench.effects.push_back(effect);

  return ench;
}

inline Enchantment CreateFlamingExplosionEnchantment() {
  Enchantment ench;
  ench.name = "Flaming Explosion";

  DamageSource source;
  source.name = "Flaming Explosion";
  source.contribute = [](std::shared_ptr<combat::CombatEventContext> ctx) {
    if (!ctx || ctx->results.empty()) {
      return rules::DamageInstance{};
    }

    auto& result = ctx->results.back();
    int damage;
    if (result.is_crit) {
      int dice = std::min(result.crit_multiplier - 1, 3);
      damage = ctx->roller->Roll(
          dice_rolls::Term{.dice_groups = {{dice, 10}, {1, 6}}});
    } else {
      damage = ctx->roller->Roll(dice_rolls::Term{.dice_groups = {{1, 6}}});
    }
    return rules::DamageInstance{
        .amount = damage,
        .types = static_cast<uint16_t>(rules::DamageType::Fire),
        .modifiers = 0};
  };
  ench.damage_sources.push_back(source);

  return ench;
}

inline Enchantment CreateVampiricEnchantment() {
  Enchantment ench;
  ench.name = "Vampiric";

  combat::Effect effect;
  effect.name = "Vampiric";
  effect.source = "Weapon Enchantment: Vampiric";
  effect.trigger = combat::CombatEvent::DealDamage;
  effect.on_event = [](std::shared_ptr<combat::CombatEventContext> ctx) {
    if (!ctx) {
      return;
    }

    if (ctx->current_index >= ctx->results.size()) {
      return;
    }
    auto current_result = ctx->results[ctx->current_index];

    int total_damage = 0;
    for (const auto& dmg : current_result.damage_instances) {
      total_damage += dmg.amount;
    }

    int temp_hp = total_damage / 2;
    if (ctx->source) {
      ctx->source->AddTempHP(temp_hp);
    }
  };
  ench.effects.push_back(effect);

  return ench;
}

inline Enchantment CreateDrainingEnchantment() {
  Enchantment ench;
  ench.name = "Draining";

  combat::Effect effect;
  effect.name = "Draining";
  effect.source = "Weapon Enchantment: Draining";
  effect.trigger = combat::CombatEvent::Hit;
  effect.on_event = [](std::shared_ptr<combat::CombatEventContext> ctx) {
    if (!ctx || !ctx->source)
      return;

    ctx->source->Heal(1);
  };
  ench.effects.push_back(effect);

  return ench;
}

inline Enchantment CreateProfaneEnchantment() {
  Enchantment ench;
  ench.name = "Profane";

  DamageSource source;
  source.name = "Profane";
  source.contribute = [](std::shared_ptr<combat::CombatEventContext> ctx) {
    if (!ctx || !ctx->target)
      return rules::DamageInstance{};

    if (static_cast<uint16_t>(ctx->target->GetAlignment()) &
        static_cast<uint16_t>(rules::Alignment::Good)) {
      return rules::DamageInstance{
          .amount =
              ctx->roller->Roll(dice_rolls::Term{.dice_groups = {{2, 6}}}),
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

#endif  // SRC_INTERNAL_ITEMS_ENCHANTMENT_LIBRARY_H_

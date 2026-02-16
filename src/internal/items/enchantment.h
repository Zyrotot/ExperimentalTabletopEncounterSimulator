// -----------------------------------------------------------------------------
// | @file      enchantment.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ITEMS_ENCHANTMENT_H_
#define SRC_INTERNAL_ITEMS_ENCHANTMENT_H_

#include <functional>
#include <string>
#include <vector>

#include "internal/combat/combat_events.h"
#include "internal/rules/damage_types.h"

namespace internal {

namespace dice_rolls {
class Roller;
}

namespace items {

using combat::CombatContext;
using combat::CombatEvent;
using rules::DamageInstance;

struct DamageSource {
  std::string name;
  std::function<DamageInstance(const CombatContext&, dice_rolls::Roller&)>
      contribute;
};

struct Effect {
  std::string name;
  CombatEvent trigger;
  std::function<void(const CombatContext&)> on;
};

struct Enchantment {
  std::string name;
  std::vector<DamageSource> damage_sources;
  std::vector<Effect> effects;
};

}  // namespace items
}  // namespace internal

#endif  // SRC_INTERNAL_ITEMS_ENCHANTMENT_H_

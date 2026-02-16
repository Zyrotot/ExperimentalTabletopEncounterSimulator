// -----------------------------------------------------------------------------
// | @file      weapon.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ITEMS_WEAPON_H_
#define SRC_INTERNAL_ITEMS_WEAPON_H_

#include <string>

#include "internal/dice_rolls/roller.h"
#include "internal/rules/damage_types.h"

namespace internal {
namespace items {

using dice_rolls::Term;
using rules::DamageModifier;
using rules::DamageType;

struct Weapon {
  std::string name;

  int attack_bonus = 0;
  Term damage = Term{};

  DamageType damage_type = DamageType::None;
  DamageModifier damage_modifier = DamageModifier::None;

  int crit_range = 0;
  int crit_multiplier = 0;
};

}  // namespace items
}  // namespace internal

#endif  // SRC_INTERNAL_ITEMS_WEAPON_H_

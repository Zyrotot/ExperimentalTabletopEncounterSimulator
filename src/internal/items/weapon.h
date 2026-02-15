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

  int attack_bonus;
  Term damage;

  DamageType damage_type;
  DamageModifier damage_modifier;

  int crit_range;
  int crit_multiplier;
};

}  // namespace items
}  // namespace internal

#endif  // SRC_INTERNAL_ITEMS_WEAPON_H_

// -----------------------------------------------------------------------------
// | @file      weapon.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ITEMS_WEAPON_H_
#define SRC_INTERNAL_ITEMS_WEAPON_H_

#include <string>
#include <vector>

#include "internal/dice_rolls/term.h"
#include "internal/items/enchantment.h"
#include "internal/rules/damage_types.h"

namespace ettes {
namespace items {

struct Weapon {
  std::string name;

  int attack_bonus = 0;
  dice_rolls::Term damage = dice_rolls::Term{};

  rules::DamageType damage_type = rules::DamageType::None;
  rules::DamageModifier damage_modifier = rules::DamageModifier::None;

  int crit_range = 0;
  int crit_multiplier = 0;

  std::vector<Enchantment> enchantments = {};
};

}  // namespace items
}  // namespace ettes

#endif  // SRC_INTERNAL_ITEMS_WEAPON_H_

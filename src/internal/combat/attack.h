// -----------------------------------------------------------------------------
// | @file      attack.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_ATTACK_H_
#define SRC_INTERNAL_COMBAT_ATTACK_H_

#include <memory>
#include <string>
#include <vector>

#include "internal/dice_rolls/roller.h"
#include "internal/items/weapon.h"

namespace internal {
namespace combat {

struct AttackMove {
  std::shared_ptr<items::Weapon> weapon;

  int attack_modifier = 0;
  int damage_modifier = 0;

  int crit_range_bonus = 0;
  int crit_multiplier_bonus = 0;

  int GetAttackModifier() const;
  dice_rolls::Term GetAttackDamage() const;
};

struct AttackSequence {
  std::string name;

  std::vector<AttackMove> attacks = {};

  int attack_modifier = 0;
  int damage_modifier = 0;
};

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_ATTACK_H_

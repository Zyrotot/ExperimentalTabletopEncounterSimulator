// -----------------------------------------------------------------------------
// | @file      attack.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_ATTACK_H_
#define SRC_INTERNAL_COMBAT_ATTACK_H_

#include <vector>

#include "internal/items/weapon.h"

namespace internal {
namespace combat {

using items::Weapon;

struct Attack {
  int attack_modifier;
  int damage_modifier;

  int crit_range_bonus;
  int crit_multiplier_bonus;
};

struct AttackMove {
  std::vector<Attack> attacks;
  int attack_modifier;
  int damage_modifier;
};

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_ATTACK_H_

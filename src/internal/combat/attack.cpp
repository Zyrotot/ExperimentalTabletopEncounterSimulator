// -----------------------------------------------------------------------------
// | @file      attack.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/combat/attack.h"

namespace internal {
namespace combat {

using dice_rolls::Term;

int AttackMove::GetAttackModifier() const {
  return attack_modifier + weapon.attack_bonus;
}

Term AttackMove::GetAttackDamage() const {
  Term total_damage = weapon.damage;
  total_damage.AddModifier(damage_modifier);

  return total_damage;
}

}  // namespace combat
}  // namespace internal

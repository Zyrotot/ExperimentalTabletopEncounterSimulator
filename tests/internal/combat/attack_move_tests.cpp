// -----------------------------------------------------------------------------
// | @file      attack_move_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/combat/attack.h"

namespace ettes {
namespace combat {

TEST(AttackMoveTest, GetAttackModifierSumsWeaponBonusAndMoveModifier) {
  items::Weapon weapon;
  weapon.attack_bonus = 5;

  AttackMove move{.weapon = weapon, .attack_modifier = 3};

  EXPECT_EQ(move.GetAttackModifier(), 8);
}

TEST(AttackMoveTest, GetAttackModifierWithZeros) {
  items::Weapon weapon;
  weapon.attack_bonus = 0;

  AttackMove move{.weapon = weapon, .attack_modifier = 0};

  EXPECT_EQ(move.GetAttackModifier(), 0);
}

TEST(AttackMoveTest, GetAttackDamageReturnsWeaponDamageWithModifier) {
  items::Weapon weapon;
  weapon.damage = dice_rolls::Term{.dice_groups = {{2, 6}}, .bonus = 0};

  AttackMove move{.weapon = weapon, .damage_modifier = 4};

  dice_rolls::Term result = move.GetAttackDamage();

  ASSERT_EQ(result.dice_groups.size(), 1u);
  EXPECT_EQ(result.dice_groups[0].count, 2);
  EXPECT_EQ(result.dice_groups[0].sides, 6);
  EXPECT_EQ(result.bonus, 4);
}

TEST(AttackMoveTest, GetAttackDamagePreservesWeaponBonus) {
  items::Weapon weapon;
  weapon.damage = dice_rolls::Term{.dice_groups = {{1, 8}}, .bonus = 3};

  AttackMove move{.weapon = weapon, .damage_modifier = 2};

  dice_rolls::Term result = move.GetAttackDamage();

  EXPECT_EQ(result.bonus, 5);
}

}  // namespace combat
}  // namespace ettes

// -----------------------------------------------------------------------------
// | @file      term_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/dice_rolls/term.h"

namespace ettes {
namespace dice_rolls {

TEST(TermTest, AddModifierIncreasesBonus) {
  Term term;
  term.bonus = 3;
  term.AddModifier(5);

  EXPECT_EQ(term.bonus, 8);
}

TEST(TermTest, AddNegativeModifierDecreasesBonus) {
  Term term;
  term.bonus = 3;
  term.AddModifier(-2);

  EXPECT_EQ(term.bonus, 1);
}

TEST(TermTest, OperatorPlusCombinesDiceGroups) {
  Term lhs;
  lhs.dice_groups = {Dice{.count = 2, .sides = 6}};
  lhs.bonus = 1;

  Term rhs;
  rhs.dice_groups = {Dice{.count = 1, .sides = 8}};
  rhs.bonus = 3;

  Term result = lhs + rhs;

  ASSERT_EQ(result.dice_groups.size(), 2u);
  EXPECT_EQ(result.dice_groups[0].count, 2);
  EXPECT_EQ(result.dice_groups[0].sides, 6);
  EXPECT_EQ(result.dice_groups[1].count, 1);
  EXPECT_EQ(result.dice_groups[1].sides, 8);
  EXPECT_EQ(result.bonus, 4);
}

TEST(TermTest, OperatorPlusWithEmptyTerms) {
  Term lhs;
  Term rhs;

  Term result = lhs + rhs;

  EXPECT_TRUE(result.dice_groups.empty());
  EXPECT_EQ(result.bonus, 0);
}

}  // namespace dice_rolls
}  // namespace ettes

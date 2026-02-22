// -----------------------------------------------------------------------------
// | @file      term_parser_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/dice_rolls/term.h"

namespace ettes {
namespace dice_rolls {

TEST(ParseTermTest, SingleDieGroupNoBonus) {
  auto result = Term::Parse("2d6");

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->dice_groups.size(), 1u);
  EXPECT_EQ(result->dice_groups[0].count, 2);
  EXPECT_EQ(result->dice_groups[0].sides, 6);
  EXPECT_EQ(result->bonus, 0);
}

TEST(ParseTermTest, SingleDieGroupWithPositiveBonus) {
  auto result = Term::Parse("4d8+5");

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->dice_groups.size(), 1u);
  EXPECT_EQ(result->dice_groups[0].count, 4);
  EXPECT_EQ(result->dice_groups[0].sides, 8);
  EXPECT_EQ(result->bonus, 5);
}

TEST(ParseTermTest, SingleDieGroupWithNegativeBonus) {
  auto result = Term::Parse("1d20-2");

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->dice_groups.size(), 1u);
  EXPECT_EQ(result->dice_groups[0].count, 1);
  EXPECT_EQ(result->dice_groups[0].sides, 20);
  EXPECT_EQ(result->bonus, -2);
}

TEST(ParseTermTest, MultipleDieGroupsWithBonus) {
  auto result = Term::Parse("2d6+1d4+3");

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->dice_groups.size(), 2u);
  EXPECT_EQ(result->dice_groups[0].count, 2);
  EXPECT_EQ(result->dice_groups[0].sides, 6);
  EXPECT_EQ(result->dice_groups[1].count, 1);
  EXPECT_EQ(result->dice_groups[1].sides, 4);
  EXPECT_EQ(result->bonus, 3);
}

TEST(ParseTermTest, EmptyStringReturnsNullopt) {
  auto result = Term::Parse("");

  EXPECT_FALSE(result.has_value());
}

TEST(ParseTermTest, NoDiceGroupsReturnsNullopt) {
  auto result = Term::Parse("invalid");

  EXPECT_FALSE(result.has_value());
}

}  // namespace dice_rolls
}  // namespace ettes

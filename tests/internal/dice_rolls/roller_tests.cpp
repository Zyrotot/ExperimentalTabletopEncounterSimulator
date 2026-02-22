// -----------------------------------------------------------------------------
// | @file      roller_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "internal/dice_rolls/roller.h"
#include "tests/mocks/mock_random_engine.h"

using ::testing::Return;

namespace internal {
namespace dice_rolls {

class RollerTest : public ::testing::Test {
 protected:
  std::unique_ptr<Roller> sut_;
};

TEST_F(RollerTest, SingleDieReturnsEngineValuePlusBonus) {
  auto mock = std::make_shared<MockRandomEngine>();
  sut_ = std::make_unique<Roller>(mock);
  EXPECT_CALL(*mock, GetRand(6)).WillOnce(Return(3));

  Term term;
  term.dice_groups = {Dice{.count = 1, .sides = 6}};
  term.bonus = 2;

  EXPECT_EQ(sut_->Roll(term), 3 + 2);
}

TEST_F(RollerTest, MultipleDiceSumsValues) {
  auto mock = std::make_shared<MockRandomEngine>();
  sut_ = std::make_unique<Roller>(mock);
  EXPECT_CALL(*mock, GetRand(6)).WillOnce(Return(2)).WillOnce(Return(4));

  Term term;
  term.dice_groups = {Dice{.count = 1, .sides = 6},
                      Dice{.count = 1, .sides = 6}};
  term.bonus = 0;

  EXPECT_EQ(sut_->Roll(term), 6);
}

TEST_F(RollerTest, UsesCorrectSidesForEachDie) {
  auto mock = std::make_shared<MockRandomEngine>();
  sut_ = std::make_unique<Roller>(mock);
  EXPECT_CALL(*mock, GetRand(8)).WillOnce(Return(1));
  EXPECT_CALL(*mock, GetRand(10)).WillOnce(Return(2));

  Term term;
  term.dice_groups = {Dice{.count = 1, .sides = 8},
                      Dice{.count = 1, .sides = 10}};
  term.bonus = 0;

  EXPECT_EQ(sut_->Roll(term), 3);
}

}  // namespace dice_rolls
}  // namespace internal

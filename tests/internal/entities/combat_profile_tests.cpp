// -----------------------------------------------------------------------------
// | @file      combat_profile_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/entities/combat_profile.h"
#include "internal/logging/log_manager.h"

namespace ettes {
namespace entities {

class CombatProfileTest : public ::testing::Test {
 protected:
  void SetUp() override {
    logger_ = logging::LogManager::GetLogger("test");
  }

  logging::Logger* logger_;
};

TEST_F(CombatProfileTest, GetAttackSequenceReturnsCorrectIndex) {
  combat::AttackSequence seq0{.name = "full_attack"};
  combat::AttackSequence seq1{.name = "secondary"};

  CombatProfile profile({}, {seq0, seq1}, logger_);

  EXPECT_EQ(profile.GetAttackSequence(0).name, "full_attack");
  EXPECT_EQ(profile.GetAttackSequence(1).name, "secondary");
}

TEST_F(CombatProfileTest, InvalidIndexFallsBackToFirstSequence) {
  combat::AttackSequence seq0{.name = "full_attack"};

  CombatProfile profile({}, {seq0}, logger_);

  EXPECT_EQ(profile.GetAttackSequence(5).name, "full_attack");
  EXPECT_EQ(profile.GetAttackSequence(-1).name, "full_attack");
}

TEST_F(CombatProfileTest, GetEquippedWeaponsReturnsAllWeapons) {
  items::Weapon w1{.name = "sword"};
  items::Weapon w2{.name = "axe"};

  CombatProfile profile({w1, w2}, {}, logger_);

  ASSERT_EQ(profile.GetEquippedWeapons().size(), 2u);
  EXPECT_EQ(profile.GetEquippedWeapons()[0].name, "sword");
  EXPECT_EQ(profile.GetEquippedWeapons()[1].name, "axe");
}

TEST_F(CombatProfileTest, GetAttackSequencesReturnsAll) {
  combat::AttackSequence seq0{.name = "primary"};
  combat::AttackSequence seq1{.name = "secondary"};

  CombatProfile profile({}, {seq0, seq1}, logger_);

  EXPECT_EQ(profile.GetAttackSequences().size(), 2u);
}

}  // namespace entities
}  // namespace ettes

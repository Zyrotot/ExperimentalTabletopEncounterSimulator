// -----------------------------------------------------------------------------
// | @file      defense_profile_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/entities/defense_profile.h"
#include "internal/logging/log_manager.h"

namespace ettes {
namespace entities {

class DefenseProfileTest : public ::testing::Test {
 protected:
  void SetUp() override {
    current_base_ = {};
    current_base_.armour_class = 20;
    current_base_.fortification = 25;
    current_bonus_ = {};
    logger_ = logging::LogManager::GetLogger("test");
    sut_ = std::make_unique<DefenseProfile>(&current_base_, &current_bonus_,
                                            logger_);
  }

  BaseStats current_base_;
  BonusStats current_bonus_;
  logging::Logger* logger_;
  std::unique_ptr<DefenseProfile> sut_;
};

TEST_F(DefenseProfileTest, GetEffectiveACReturnsBaseAC) {
  EXPECT_EQ(sut_->GetEffectiveAC(), 20);
}

TEST_F(DefenseProfileTest, GetEffectiveACIncludesBonusAC) {
  current_bonus_.ac_bonus = 5;

  EXPECT_EQ(sut_->GetEffectiveAC(), 25);
}

TEST_F(DefenseProfileTest, GetFortificationReturnsBaseValue) {
  EXPECT_EQ(sut_->GetFortification(), 25);
}

TEST_F(DefenseProfileTest, GetResistancesCombinesBaseAndBonus) {
  current_base_.resistances.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 5});
  current_bonus_.bonus_resistances.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 3});

  Resistances result = sut_->GetResistances();

  EXPECT_EQ(result.damage_reductions.size(), 2u);
}

TEST_F(DefenseProfileTest, AddDRToBase) {
  rules::DamageReduction dr{rules::DamageType::None,
                            rules::DamageModifier::None, 5};
  sut_->AddDR(dr, false);

  EXPECT_EQ(current_base_.resistances.damage_reductions.size(), 1u);
  EXPECT_EQ(current_base_.resistances.damage_reductions[0].amount, 5);
}

TEST_F(DefenseProfileTest, AddDRToBonus) {
  rules::DamageReduction dr{rules::DamageType::None,
                            rules::DamageModifier::None, 5};
  sut_->AddDR(dr, true);

  EXPECT_EQ(current_bonus_.bonus_resistances.damage_reductions.size(), 1u);
  EXPECT_EQ(current_bonus_.bonus_resistances.damage_reductions[0].amount, 5);
}

TEST_F(DefenseProfileTest, AddDRStacksWithMatchingBypass) {
  rules::DamageReduction dr{rules::DamageType::None,
                            rules::DamageModifier::None, 5};
  sut_->AddDR(dr, true);
  sut_->AddDR(dr, true);

  EXPECT_EQ(current_bonus_.bonus_resistances.damage_reductions.size(), 1u);
  EXPECT_EQ(current_bonus_.bonus_resistances.damage_reductions[0].amount, 10);
}

TEST_F(DefenseProfileTest, RemoveDRFromBonus) {
  rules::DamageReduction dr{rules::DamageType::None,
                            rules::DamageModifier::None, 10};
  sut_->AddDR(dr, true);
  sut_->RemoveDR(6, true);

  EXPECT_EQ(current_bonus_.bonus_resistances.damage_reductions.size(), 1u);
  EXPECT_EQ(current_bonus_.bonus_resistances.damage_reductions[0].amount, 4);
}

TEST_F(DefenseProfileTest, RemoveDRFullyRemovesEntry) {
  rules::DamageReduction dr{rules::DamageType::None,
                            rules::DamageModifier::None, 5};
  sut_->AddDR(dr, true);
  sut_->RemoveDR(5, true);

  EXPECT_TRUE(current_bonus_.bonus_resistances.damage_reductions.empty());
}

TEST_F(DefenseProfileTest, ClearAllDRFromBonus) {
  rules::DamageReduction dr1{rules::DamageType::None,
                             rules::DamageModifier::None, 5};
  rules::DamageReduction dr2{rules::DamageType::Slash,
                             rules::DamageModifier::None, 3};
  sut_->AddDR(dr1, true);
  sut_->AddDR(dr2, true);
  sut_->ClearAllDR(true);

  EXPECT_TRUE(current_bonus_.bonus_resistances.damage_reductions.empty());
}

TEST_F(DefenseProfileTest, ClearAllDRFromBase) {
  rules::DamageReduction dr{rules::DamageType::None,
                            rules::DamageModifier::None, 5};
  sut_->AddDR(dr, false);
  sut_->ClearAllDR(false);

  EXPECT_TRUE(current_base_.resistances.damage_reductions.empty());
}

TEST_F(DefenseProfileTest, GetResistancesCachesResult) {
  current_base_.resistances.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 5});

  Resistances first_call = sut_->GetResistances();
  Resistances second_call = sut_->GetResistances();

  EXPECT_EQ(first_call.damage_reductions.size(),
            second_call.damage_reductions.size());
}

TEST_F(DefenseProfileTest, AddDRInvalidatesCacheSuccessfully) {
  Resistances before = sut_->GetResistances();
  EXPECT_TRUE(before.damage_reductions.empty());

  rules::DamageReduction dr{rules::DamageType::None,
                            rules::DamageModifier::None, 5};
  sut_->AddDR(dr, true);

  Resistances after = sut_->GetResistances();
  EXPECT_EQ(after.damage_reductions.size(), 1u);
}

}  // namespace entities
}  // namespace ettes

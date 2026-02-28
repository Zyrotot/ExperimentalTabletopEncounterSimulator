// -----------------------------------------------------------------------------
// | @file      health_component_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/entities/health_component.h"

namespace ettes {
namespace entities {

class HealthComponentTest : public ::testing::Test {
 protected:
  void SetUp() override {
    starting_base_.hp = 100;
    current_base_ = starting_base_;
    current_bonus_ = {};
    sut_ = std::make_unique<HealthComponent>(&current_base_, &current_bonus_,
                                             &starting_base_);
  }

  BaseStats starting_base_;
  BaseStats current_base_;
  BonusStats current_bonus_;
  std::unique_ptr<HealthComponent> sut_;
};

TEST_F(HealthComponentTest, StartsAlive) {
  EXPECT_TRUE(sut_->IsAlive());
}

TEST_F(HealthComponentTest, TakeDamageReducesHP) {
  sut_->TakeDamage(30);

  EXPECT_EQ(current_base_.hp, 70);
  EXPECT_TRUE(sut_->IsAlive());
}

TEST_F(HealthComponentTest, TakeLethalDamageSetsHPToZero) {
  sut_->TakeDamage(150);

  EXPECT_EQ(current_base_.hp, 0);
  EXPECT_FALSE(sut_->IsAlive());
}

TEST_F(HealthComponentTest, TakeZeroDamageDoesNothing) {
  sut_->TakeDamage(0);

  EXPECT_EQ(current_base_.hp, 100);
}

TEST_F(HealthComponentTest, TakeNegativeDamageDoesNothing) {
  sut_->TakeDamage(-5);

  EXPECT_EQ(current_base_.hp, 100);
}

TEST_F(HealthComponentTest, HealRestoresHP) {
  sut_->TakeDamage(40);
  sut_->Heal(20);

  EXPECT_EQ(current_base_.hp, 80);
}

TEST_F(HealthComponentTest, HealDoesNotExceedMaxHP) {
  sut_->TakeDamage(10);
  sut_->Heal(50);

  EXPECT_EQ(current_base_.hp, 100);
}

TEST_F(HealthComponentTest, HealWithZeroDoesNothing) {
  sut_->TakeDamage(10);
  sut_->Heal(0);

  EXPECT_EQ(current_base_.hp, 90);
}

TEST_F(HealthComponentTest, AddTempHPSetsTemporaryHP) {
  sut_->AddTempHP(20);

  EXPECT_EQ(current_bonus_.temporary_hp, 20);
}

TEST_F(HealthComponentTest, AddTempHPKeepsHigherValue) {
  sut_->AddTempHP(20);
  sut_->AddTempHP(10);

  EXPECT_EQ(current_bonus_.temporary_hp, 20);
}

TEST_F(HealthComponentTest, AddTempHPReplacesWithHigherValue) {
  sut_->AddTempHP(10);
  sut_->AddTempHP(20);

  EXPECT_EQ(current_bonus_.temporary_hp, 20);
}

TEST_F(HealthComponentTest, TempHPAbsorbsDamageFirst) {
  sut_->AddTempHP(15);
  sut_->TakeDamage(10);

  EXPECT_EQ(current_bonus_.temporary_hp, 5);
  EXPECT_EQ(current_base_.hp, 100);
}

TEST_F(HealthComponentTest, DamageOverflowsTempHPIntoHP) {
  sut_->AddTempHP(10);
  sut_->TakeDamage(25);

  EXPECT_EQ(current_bonus_.temporary_hp, 0);
  EXPECT_EQ(current_base_.hp, 85);
}

TEST_F(HealthComponentTest, ExactlyLethalDamageKills) {
  sut_->TakeDamage(100);

  EXPECT_EQ(current_base_.hp, 0);
  EXPECT_FALSE(sut_->IsAlive());
}

}  // namespace entities
}  // namespace ettes

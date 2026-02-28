// -----------------------------------------------------------------------------
// | @file      damage_resolver_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/logging/log_manager.h"
#include "internal/resolver/damage_resolver.h"

namespace ettes {
namespace resolver {

class ApplyResistancesTest : public ::testing::Test {
 protected:
  void SetUp() override {
    logger_ = logging::LogManager::GetLogger("test");
  }

  logging::Logger* logger_;
};

TEST_F(ApplyResistancesTest, NoDRDoesNotReduceDamage) {
  rules::DamageInstance dmg{
      .amount = 20,
      .types = static_cast<uint16_t>(rules::DamageType::Slash),
      .modifiers = 0};
  entities::Resistances res;

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 20);
}

TEST_F(ApplyResistancesTest, DamageReductionReducesPhysicalDamage) {
  rules::DamageInstance dmg{
      .amount = 20,
      .types = static_cast<uint16_t>(rules::DamageType::Slash),
      .modifiers = 0};
  entities::Resistances res;
  res.damage_reductions.push_back({rules::DamageType::None,
                                   rules::DamageModifier::None, 5});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 15);
}


TEST_F(ApplyResistancesTest, DamageReductionIsBypassed) {
  rules::DamageInstance dmg{
      .amount = 20,
      .types = static_cast<uint16_t>(rules::DamageType::Slash),
      .modifiers = 0};
  entities::Resistances res;
  res.damage_reductions.push_back({rules::DamageType::Slash,
                                   rules::DamageModifier::None, 5});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 20);
}

TEST_F(ApplyResistancesTest, DRFullyAbsorbsDamage) {
  rules::DamageInstance dmg{
      .amount = 3,
      .types = static_cast<uint16_t>(rules::DamageType::Slash),
      .modifiers = 0};
  entities::Resistances res;
  res.damage_reductions.push_back({rules::DamageType::None,
                                   rules::DamageModifier::None, 10});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 0);
}

TEST_F(ApplyResistancesTest, ImmunityNegatesAllDamage) {
  rules::DamageInstance dmg{
      .amount = 50,
      .types = static_cast<uint16_t>(rules::DamageType::Fire),
      .modifiers = 0};
  entities::Resistances res;
  res.immunity.immune_types = rules::DamageType::Fire;

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 0);
}

TEST_F(ApplyResistancesTest, EnergyResistanceReducesEnergyDamage) {
  rules::DamageInstance dmg{
      .amount = 20,
      .types = static_cast<uint16_t>(rules::DamageType::Fire),
      .modifiers = 0};
  entities::Resistances res;
  res.energy_resistances.push_back({rules::DamageType::Fire, 8});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 12);
}

TEST_F(ApplyResistancesTest, EnergyResistanceFullyAbsorbsDamage) {
  rules::DamageInstance dmg{
      .amount = 5,
      .types = static_cast<uint16_t>(rules::DamageType::Cold),
      .modifiers = 0};
  entities::Resistances res;
  res.energy_resistances.push_back({rules::DamageType::Cold, 10});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 0);
}

TEST_F(ApplyResistancesTest, EnergyResistanceIsOfDifferentType) {
  rules::DamageInstance dmg{
      .amount = 5,
      .types = static_cast<uint16_t>(rules::DamageType::Cold),
      .modifiers = 0};
  entities::Resistances res;
  res.energy_resistances.push_back({rules::DamageType::Fire, 10});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 5);
}

TEST_F(ApplyResistancesTest, WeaknessIncreasesDamage) {
  rules::DamageInstance dmg{
      .amount = 10,
      .types = static_cast<uint16_t>(rules::DamageType::Fire),
      .modifiers = 0};
  entities::Resistances res;
  res.weaknesses.push_back({rules::DamageType::Fire, 5});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 15);
}

TEST_F(ApplyResistancesTest, DRDoesNotApplyToEnergyDamage) {
  rules::DamageInstance dmg{
      .amount = 20,
      .types = static_cast<uint16_t>(rules::DamageType::Fire),
      .modifiers = 0};
  entities::Resistances res;
  res.damage_reductions.push_back({rules::DamageType::None,
                                   rules::DamageModifier::None, 10});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 20);
}

TEST_F(ApplyResistancesTest, EnergyResistanceDoesNotApplyToPhysicalDamage) {
  rules::DamageInstance dmg{
      .amount = 20,
      .types = static_cast<uint16_t>(rules::DamageType::Slash),
      .modifiers = 0};
  entities::Resistances res;
  res.energy_resistances.push_back({rules::DamageType::Fire, 10});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 20);
}

TEST_F(ApplyResistancesTest, DRIsConsumedAcrossMultipleInstances) {
  rules::DamageInstance dmg1{
      .amount = 8,
      .types = static_cast<uint16_t>(rules::DamageType::Slash),
      .modifiers = 0};
  rules::DamageInstance dmg2{
      .amount = 8,
      .types = static_cast<uint16_t>(rules::DamageType::Pierce),
      .modifiers = 0};
  entities::Resistances res;
  res.damage_reductions.push_back({rules::DamageType::None,
                                   rules::DamageModifier::None, 10});

  DamageResolver::ApplyResistancesToDamage(&dmg1, &res, logger_);
  DamageResolver::ApplyResistancesToDamage(&dmg2, &res, logger_);

  EXPECT_EQ(dmg1.amount, 0);
  EXPECT_EQ(dmg2.amount, 6);
}

}  // namespace resolver
}  // namespace ettes

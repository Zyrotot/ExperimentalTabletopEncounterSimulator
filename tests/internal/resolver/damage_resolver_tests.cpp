// -----------------------------------------------------------------------------
// | @file      damage_resolver_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "internal/combat/attack_queue.h"
#include "internal/dice_rolls/roller.h"
#include "internal/logging/log_manager.h"
#include "internal/resolver/damage_resolver.h"
#include "tests/mocks/mock_entity.h"
#include "tests/mocks/mock_random_engine.h"

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

TEST_F(ApplyResistancesTest, DRBypassedByModifier) {
  rules::DamageInstance dmg{
      .amount = 20,
      .types = static_cast<uint16_t>(rules::DamageType::Slash),
      .modifiers = static_cast<uint16_t>(rules::DamageModifier::Magic)};
  entities::Resistances res;
  res.damage_reductions.push_back({rules::DamageType::None,
                                   rules::DamageModifier::Magic, 5});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 20);
}

TEST_F(ApplyResistancesTest, DRWithZeroAmountSkipped) {
  rules::DamageInstance dmg{
      .amount = 20,
      .types = static_cast<uint16_t>(rules::DamageType::Slash),
      .modifiers = 0};
  entities::Resistances res;
  res.damage_reductions.push_back({rules::DamageType::None,
                                   rules::DamageModifier::None, 0});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 20);
}

TEST_F(ApplyResistancesTest, WeaknessDoesNotMatchType) {
  rules::DamageInstance dmg{
      .amount = 10,
      .types = static_cast<uint16_t>(rules::DamageType::Cold),
      .modifiers = 0};
  entities::Resistances res;
  res.weaknesses.push_back({rules::DamageType::Fire, 5});

  DamageResolver::ApplyResistancesToDamage(&dmg, &res, logger_);

  EXPECT_EQ(dmg.amount, 10);
}

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;

class MockAttackQueue : public combat::IAttackQueue {
 public:
  MOCK_METHOD(void, QueueAttack, (combat::QueuedAttack), (override));
  MOCK_METHOD(void, NotifyEntityDied, (entities::IEntity*), (override));
};

class ApplySingleAttackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_engine_ = std::make_shared<dice_rolls::MockRandomEngine>();
    roller_ = std::make_shared<dice_rolls::Roller>(mock_engine_);
    source_ = std::make_unique<MockEntity>();
    target_ = std::make_unique<MockEntity>();

    ON_CALL(*source_, GetName()).WillByDefault(ReturnRef(source_name_));
    ON_CALL(*target_, GetName()).WillByDefault(ReturnRef(target_name_));
    ON_CALL(*source_, GetEffectsForEvent(_))
        .WillByDefault(ReturnRef(empty_effects_));
    ON_CALL(*target_, GetEffectsForEvent(_))
        .WillByDefault(ReturnRef(empty_effects_));

    context_.source = source_.get();
    context_.target = target_.get();
    context_.roller = roller_.get();
  }

  std::string source_name_ = "Attacker";
  std::string target_name_ = "Target";
  std::shared_ptr<dice_rolls::MockRandomEngine> mock_engine_;
  std::shared_ptr<dice_rolls::Roller> roller_;
  std::unique_ptr<MockEntity> source_;
  std::unique_ptr<MockEntity> target_;
  combat::CombatEventContext context_;
  std::vector<const combat::Effect*> empty_effects_;
};

TEST_F(ApplySingleAttackTest, OutOfBoundsIndexDoesNothing) {
  DamageResolver resolver(&context_);

  resolver.ApplySingleAttack(99);
}

TEST_F(ApplySingleAttackTest, MissDoesNotApplyDamage) {
  context_.results.push_back(
      {.attack = nullptr, .is_hit = false, .damage_instances = {}});

  EXPECT_CALL(*target_, TakeDamage(_)).Times(0);

  DamageResolver resolver(&context_);
  resolver.ApplySingleAttack(0);
}

TEST_F(ApplySingleAttackTest, HitAppliesDamageToTarget) {
  context_.results.push_back(
      {.attack = nullptr,
       .is_hit = true,
       .damage_instances = {{.amount = 15,
                             .types = static_cast<uint16_t>(
                                 rules::DamageType::Slash),
                             .modifiers = 0}}});

  entities::Resistances empty_res;
  ON_CALL(*target_, GetResistances()).WillByDefault(Return(empty_res));
  ON_CALL(*target_, IsAlive()).WillByDefault(Return(true));
  EXPECT_CALL(*target_, TakeDamage(15));

  DamageResolver resolver(&context_);
  resolver.ApplySingleAttack(0);
}

TEST_F(ApplySingleAttackTest, ZeroDamageDoesNotCallTakeDamage) {
  context_.results.push_back(
      {.attack = nullptr,
       .is_hit = true,
       .damage_instances = {{.amount = 0,
                             .types = static_cast<uint16_t>(
                                 rules::DamageType::Slash),
                             .modifiers = 0}}});

  entities::Resistances empty_res;
  ON_CALL(*target_, GetResistances()).WillByDefault(Return(empty_res));
  EXPECT_CALL(*target_, TakeDamage(_)).Times(0);

  DamageResolver resolver(&context_);
  resolver.ApplySingleAttack(0);
}

TEST_F(ApplySingleAttackTest, HitKillsTargetNotifiesAttackQueue) {
  context_.results.push_back(
      {.attack = nullptr,
       .is_hit = true,
       .damage_instances = {{.amount = 100,
                             .types = static_cast<uint16_t>(
                                 rules::DamageType::Slash),
                             .modifiers = 0}}});

  MockAttackQueue mock_queue;
  context_.attack_queue = &mock_queue;

  entities::Resistances empty_res;
  ON_CALL(*target_, GetResistances()).WillByDefault(Return(empty_res));
  EXPECT_CALL(*target_, IsAlive())
      .WillOnce(Return(true))
      .WillOnce(Return(false));
  EXPECT_CALL(*target_, TakeDamage(100));
  EXPECT_CALL(mock_queue, NotifyEntityDied(target_.get()));

  DamageResolver resolver(&context_);
  resolver.ApplySingleAttack(0);
}

TEST_F(ApplySingleAttackTest, HitKillsTargetWithoutAttackQueue) {
  context_.results.push_back(
      {.attack = nullptr,
       .is_hit = true,
       .damage_instances = {{.amount = 100,
                             .types = static_cast<uint16_t>(
                                 rules::DamageType::Slash),
                             .modifiers = 0}}});

  context_.attack_queue = nullptr;

  entities::Resistances empty_res;
  ON_CALL(*target_, GetResistances()).WillByDefault(Return(empty_res));
  EXPECT_CALL(*target_, IsAlive())
      .WillOnce(Return(true))
      .WillOnce(Return(false));
  EXPECT_CALL(*target_, TakeDamage(100));

  DamageResolver resolver(&context_);
  resolver.ApplySingleAttack(0);
}

TEST_F(ApplySingleAttackTest, SelfDamageAppliedToSource) {
  context_.results.push_back(
      {.attack = nullptr,
       .is_hit = true,
       .damage_instances = {{.amount = 10,
                             .types = static_cast<uint16_t>(
                                 rules::DamageType::Slash),
                             .modifiers = 0}}});
  context_.self_damage_instances.push_back(
      {.amount = 5,
       .types = static_cast<uint16_t>(rules::DamageType::Fire),
       .modifiers = 0});

  entities::Resistances empty_res;
  ON_CALL(*target_, GetResistances()).WillByDefault(Return(empty_res));
  ON_CALL(*target_, IsAlive()).WillByDefault(Return(true));
  EXPECT_CALL(*target_, TakeDamage(10));

  ON_CALL(*source_, GetResistances()).WillByDefault(Return(empty_res));
  ON_CALL(*source_, IsAlive()).WillByDefault(Return(true));
  EXPECT_CALL(*source_, TakeDamage(5));

  DamageResolver resolver(&context_);
  resolver.ApplySingleAttack(0);
}

TEST_F(ApplySingleAttackTest, SelfDamageKillsSourceNotifiesQueue) {
  context_.results.push_back(
      {.attack = nullptr,
       .is_hit = true,
       .damage_instances = {{.amount = 5,
                             .types = static_cast<uint16_t>(
                                 rules::DamageType::Slash),
                             .modifiers = 0}}});
  context_.self_damage_instances.push_back(
      {.amount = 50,
       .types = static_cast<uint16_t>(rules::DamageType::Fire),
       .modifiers = 0});

  MockAttackQueue mock_queue;
  context_.attack_queue = &mock_queue;

  entities::Resistances empty_res;
  ON_CALL(*target_, GetResistances()).WillByDefault(Return(empty_res));
  ON_CALL(*target_, IsAlive()).WillByDefault(Return(true));
  EXPECT_CALL(*target_, TakeDamage(5));

  ON_CALL(*source_, GetResistances()).WillByDefault(Return(empty_res));
  EXPECT_CALL(*source_, IsAlive())
      .WillOnce(Return(true))
      .WillOnce(Return(false));
  EXPECT_CALL(*source_, TakeDamage(50));
  EXPECT_CALL(mock_queue, NotifyEntityDied(source_.get()));

  DamageResolver resolver(&context_);
  resolver.ApplySingleAttack(0);
}

TEST_F(ApplySingleAttackTest, SelfDamageZeroAfterResistancesNotApplied) {
  context_.results.push_back(
      {.attack = nullptr,
       .is_hit = true,
       .damage_instances = {{.amount = 5,
                             .types = static_cast<uint16_t>(
                                 rules::DamageType::Slash),
                             .modifiers = 0}}});
  context_.self_damage_instances.push_back(
      {.amount = 5,
       .types = static_cast<uint16_t>(rules::DamageType::Fire),
       .modifiers = 0});

  entities::Resistances empty_res;
  ON_CALL(*target_, GetResistances()).WillByDefault(Return(empty_res));
  ON_CALL(*target_, IsAlive()).WillByDefault(Return(true));
  EXPECT_CALL(*target_, TakeDamage(5));

  entities::Resistances source_res;
  source_res.immunity.immune_types = rules::DamageType::Fire;
  ON_CALL(*source_, GetResistances()).WillByDefault(Return(source_res));
  EXPECT_CALL(*source_, TakeDamage(_)).Times(0);

  DamageResolver resolver(&context_);
  resolver.ApplySingleAttack(0);
}

}  // namespace resolver
}  // namespace ettes

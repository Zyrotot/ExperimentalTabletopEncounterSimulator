#include <gtest/gtest.h>

#include <memory>

#include "src/internal/abilities/ability.h"
#include "src/internal/entities/entity.h"
#include "tests/integration/test_helpers.h"

using ::testing::Return;

namespace ettes {
namespace testing {

class DuroDeFerirTest : public ::testing::Test {
 protected:
  void SetUp() override {
    harness_ = std::make_unique<TestHarness>();
  }

  std::unique_ptr<TestHarness> harness_;
};

TEST_F(DuroDeFerirTest, NegatesAnyIncomingDamageOnce) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 30, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeFerir(1)});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 30);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 0);
}

TEST_F(DuroDeFerirTest, DoesNotTriggerAfterStackDepleted) {
  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 2);
  auto target_cfg = MakeEntityConfig("Target", 100, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeFerir(1)});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));

    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 87);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 0);
}

TEST_F(DuroDeFerirTest, MultipleStacksNegateMultipleHits) {
  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 3);
  auto target_cfg = MakeEntityConfig("Target", 100, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeFerir(3)});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    for (int i = 0; i < 3; ++i) {
      EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
      EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
    }
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 100);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 0);
}

TEST_F(DuroDeFerirTest, NegatesEvenMassiveOverkillDamage) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeGreataxe());
  auto target_cfg = MakeEntityConfig("Target", 10, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeFerir(1)});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(20));
    EXPECT_CALL(*harness_->mock_engine, GetRand(12)).WillOnce(Return(12));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 10);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 0);
}

TEST_F(DuroDeFerirTest, CombinedWithDR_DuroDeFerir_TakesPriority) {
  entities::Resistances res;
  res.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 5});

  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 2);
  auto target_cfg = MakeEntityConfig("Target", 30, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeFerir(1)}, res);
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 22);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 0);
}

}  // namespace testing
}  // namespace ettes

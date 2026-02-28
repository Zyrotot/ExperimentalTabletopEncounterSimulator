#include <gtest/gtest.h>

#include <memory>

#include "src/internal/abilities/ability.h"
#include "src/internal/entities/entity.h"
#include "tests/integration/test_helpers.h"

using ::testing::Return;

namespace ettes {
namespace testing {

class DuroDeMatarTest : public ::testing::Test {
 protected:
  void SetUp() override {
    harness_ = std::make_unique<TestHarness>();
  }

  std::unique_ptr<TestHarness> harness_;
};

TEST_F(DuroDeMatarTest, SurvivesLethalHitConsumesOneStack) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 10, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeMatar(1)});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_TRUE(target.IsAlive());
  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 10);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeMatar), 0);
}

TEST_F(DuroDeMatarTest, DoesNotTriggerOnNonLethalHit) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 50, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeMatar(1)});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(5));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 40);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeMatar), 1);
}

TEST_F(DuroDeMatarTest, SecondLethalHitKillsAfterStackExhausted) {
  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 2);
  auto target_cfg = MakeEntityConfig("Target", 10, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeMatar(1)});
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

  EXPECT_FALSE(target.IsAlive());
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeMatar), 0);
}

TEST_F(DuroDeMatarTest, MultipleStacksSurviveMultipleLethalHits) {
  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 2);
  auto target_cfg = MakeEntityConfig("Target", 10, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeMatar(2)});
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

  EXPECT_TRUE(target.IsAlive());
  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 10);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeMatar), 0);
}

TEST_F(DuroDeMatarTest, WorksWithTempHPInThreshold) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 5, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeMatar(1)});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);
  target.AddTempHP(5);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_TRUE(target.IsAlive());
  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 5);
  EXPECT_EQ(target.GetCurrentStats().bonus_stats.temporary_hp, 5);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeMatar), 0);
}

TEST_F(DuroDeMatarTest, ExactLethalDamageTriggers) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 13, 10, 0, MakeLongsword(),
                                     {abilities::CreateDuroDeMatar(1)});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_TRUE(target.IsAlive());
  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 13);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeMatar), 0);
}

}  // namespace testing
}  // namespace ettes

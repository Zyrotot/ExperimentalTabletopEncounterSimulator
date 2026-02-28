#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "src/internal/abilities/ability.h"
#include "src/internal/combat/attack_result.h"
#include "src/internal/engine/combat_engine.h"
#include "src/internal/engine/director.h"
#include "src/internal/engine/encounter.h"
#include "src/internal/entities/entity.h"
#include "tests/integration/test_helpers.h"

using ::testing::Return;

namespace ettes {
namespace testing {

class TrespassarTest : public ::testing::Test {
 protected:
  void SetUp() override {
    harness_ = std::make_unique<TestHarness>();
  }

  std::unique_ptr<TestHarness> harness_;
};

TEST_F(TrespassarTest, KillingBlowQueuesExtraAttack) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword(),
                                       {abilities::CreateTrespassar()});

  auto target_a_cfg = MakeEntityConfig("TargetA", 5, 10, 0, MakeLongsword());
  auto target_b_cfg = MakeEntityConfig("TargetB", 50, 10, 0, MakeLongsword());

  auto attacker_ptr = std::make_unique<entities::Entity>(attacker_cfg);
  auto target_a_ptr = std::make_unique<entities::Entity>(target_a_cfg);
  auto target_b_ptr = std::make_unique<entities::Entity>(target_b_cfg);

  entities::Entity* attacker_raw = attacker_ptr.get();
  entities::Entity* target_b_raw = target_b_ptr.get();

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(attacker_ptr));
  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(target_a_ptr));
  side_b.push_back(std::move(target_b_ptr));

  engine::Encounter encounter(std::move(side_a), std::move(side_b));
  engine::Director director(&encounter, harness_->engine.get());

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));

    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  director.RunTurn(attacker_raw);

  EXPECT_TRUE(target_b_raw->IsAlive());
  EXPECT_EQ(target_b_raw->GetCurrentStats().base_stats.hp, 37);
  EXPECT_EQ(attacker_raw->GetAbilityStack(abilities::AbilityId::Trespassar), 0);
}

TEST_F(TrespassarTest, NoExtraAttackIfStackIsZero) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword(),
                                       {abilities::CreateTrespassar()});
  auto target_cfg = MakeEntityConfig("Target", 5, 10, 0, MakeLongsword());

  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  attacker.SetAbilityStack(abilities::AbilityId::Trespassar, 0);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_FALSE(target.IsAlive());
  EXPECT_EQ(attacker.GetAbilityStack(abilities::AbilityId::Trespassar), 0);
}

TEST_F(TrespassarTest, StackResetsOnTurnStart) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword(),
                                       {abilities::CreateTrespassar()});

  auto target_a_cfg = MakeEntityConfig("TargetA", 5, 10, 0, MakeLongsword());
  auto target_b_cfg = MakeEntityConfig("TargetB", 200, 10, 0, MakeLongsword());

  auto attacker_ptr = std::make_unique<entities::Entity>(attacker_cfg);
  auto target_a_ptr = std::make_unique<entities::Entity>(target_a_cfg);
  auto target_b_ptr = std::make_unique<entities::Entity>(target_b_cfg);

  entities::Entity* attacker_raw = attacker_ptr.get();
  entities::Entity* target_b_raw = target_b_ptr.get();

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(attacker_ptr));
  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(target_a_ptr));
  side_b.push_back(std::move(target_b_ptr));

  engine::Encounter encounter(std::move(side_a), std::move(side_b));
  engine::Director director(&encounter, harness_->engine.get());

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));

    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(4));
  }

  director.RunTurn(attacker_raw);
  EXPECT_EQ(attacker_raw->GetAbilityStack(abilities::AbilityId::Trespassar), 0);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(4));
  }

  director.RunTurn(attacker_raw);
  EXPECT_EQ(attacker_raw->GetAbilityStack(abilities::AbilityId::Trespassar), 1);
}

TEST_F(TrespassarTest, ChainKillDoesNotChain) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword(),
                                       {abilities::CreateTrespassar()});

  auto target_a_cfg = MakeEntityConfig("TargetA", 5, 10, 0, MakeLongsword());
  auto target_b_cfg = MakeEntityConfig("TargetB", 5, 10, 0, MakeLongsword());
  auto target_c_cfg = MakeEntityConfig("TargetC", 50, 10, 0, MakeLongsword());

  auto attacker_ptr = std::make_unique<entities::Entity>(attacker_cfg);
  auto target_a_ptr = std::make_unique<entities::Entity>(target_a_cfg);
  auto target_b_ptr = std::make_unique<entities::Entity>(target_b_cfg);
  auto target_c_ptr = std::make_unique<entities::Entity>(target_c_cfg);

  entities::Entity* attacker_raw = attacker_ptr.get();
  entities::Entity* target_c_raw = target_c_ptr.get();

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(attacker_ptr));
  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(target_a_ptr));
  side_b.push_back(std::move(target_b_ptr));
  side_b.push_back(std::move(target_c_ptr));

  engine::Encounter encounter(std::move(side_a), std::move(side_b));
  engine::Director director(&encounter, harness_->engine.get());

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));

    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  director.RunTurn(attacker_raw);

  EXPECT_EQ(target_c_raw->GetCurrentStats().base_stats.hp, 50);
  EXPECT_EQ(attacker_raw->GetAbilityStack(abilities::AbilityId::Trespassar), 0);
}

TEST_F(TrespassarTest, NonKillingHitDoesNotTrigger) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword(),
                                       {abilities::CreateTrespassar()});

  auto target_a_cfg = MakeEntityConfig("TargetA", 200, 10, 0, MakeLongsword());
  auto target_b_cfg = MakeEntityConfig("TargetB", 50, 10, 0, MakeLongsword());

  auto attacker_ptr = std::make_unique<entities::Entity>(attacker_cfg);
  auto target_a_ptr = std::make_unique<entities::Entity>(target_a_cfg);
  auto target_b_ptr = std::make_unique<entities::Entity>(target_b_cfg);

  entities::Entity* attacker_raw = attacker_ptr.get();
  entities::Entity* target_b_raw = target_b_ptr.get();

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(attacker_ptr));
  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(target_a_ptr));
  side_b.push_back(std::move(target_b_ptr));

  engine::Encounter encounter(std::move(side_a), std::move(side_b));
  engine::Director director(&encounter, harness_->engine.get());

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(4));
  }

  director.RunTurn(attacker_raw);

  EXPECT_EQ(target_b_raw->GetCurrentStats().base_stats.hp, 50);
  EXPECT_EQ(attacker_raw->GetAbilityStack(abilities::AbilityId::Trespassar), 1);
}

}  // namespace testing
}  // namespace ettes

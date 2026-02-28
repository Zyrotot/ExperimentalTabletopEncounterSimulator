#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "src/internal/abilities/ability.h"
#include "src/internal/combat/combat_context.h"
#include "src/internal/combat/event_manager.h"
#include "src/internal/engine/combat_engine.h"
#include "src/internal/engine/director.h"
#include "src/internal/engine/encounter.h"
#include "src/internal/entities/entity.h"
#include "src/internal/rules/resistances.h"
#include "tests/integration/test_helpers.h"

using ::testing::Return;

namespace ettes {
namespace testing {

class MultiAbilityInteractionTest : public ::testing::Test {
 protected:
  void SetUp() override {
    harness_ = std::make_unique<TestHarness>();
  }

  std::unique_ptr<TestHarness> harness_;
};

TEST_F(MultiAbilityInteractionTest, ErosaoAndDRTarget_ErodeThenDamage) {
  entities::Resistances res;
  res.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 5});

  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 2,
                                  {abilities::CreateErosao()});
  auto target_cfg =
      MakeEntityConfig("Target", 200, 10, 0, MakeLongsword(), {}, res);
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

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 181);
}

TEST_F(MultiAbilityInteractionTest, RigidezRaivosaVsErosao_DRRace) {
  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 3,
                                  {abilities::CreateErosao()});
  auto target_cfg = MakeEntityConfig("Target", 200, 10, 0, MakeLongsword(),
                                     {abilities::CreateRigidezRaivosa()});
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

  EXPECT_TRUE(target.IsAlive());
  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 161);
}

TEST_F(MultiAbilityInteractionTest, TrespassarPlusDuroDeMatarOnTarget) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword(),
                                       {abilities::CreateTrespassar()});
  auto target_a_cfg = MakeEntityConfig("TargetA", 5, 10, 0, MakeLongsword());
  auto target_b_cfg = MakeEntityConfig("TargetB", 10, 10, 0, MakeLongsword(),
                                       {abilities::CreateDuroDeMatar(1)});

  auto attacker_ptr = std::make_unique<entities::Entity>(attacker_cfg);
  auto target_a_ptr = std::make_unique<entities::Entity>(target_a_cfg);
  auto target_b_ptr = std::make_unique<entities::Entity>(target_b_cfg);

  entities::Entity* target_b_raw = target_b_ptr.get();
  entities::Entity* attacker_raw = attacker_ptr.get();

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
  EXPECT_EQ(target_b_raw->GetCurrentStats().base_stats.hp, 10);
  EXPECT_EQ(
      target_b_raw->GetAbilityStack(abilities::AbilityId::DuroDeMatar), 0);
}

TEST_F(MultiAbilityInteractionTest, DuroDeFerirPlusRigidezRaivosa) {
  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 2);
  auto target_cfg =
      MakeEntityConfig("Target", 100, 10, 0, MakeLongsword(),
                        {abilities::CreateDuroDeFerir(1),
                         abilities::CreateRigidezRaivosa()});
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

  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 0);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 87);
}

TEST_F(MultiAbilityInteractionTest, CritFortErosaoAndDR) {
  entities::Resistances res;
  res.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 5});

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword(),
                                       {abilities::CreateErosao()});
  auto target_cfg =
      MakeEntityConfig("Target", 200, 10, 100, MakeLongsword(), {}, res);
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(20));
    EXPECT_CALL(*harness_->mock_engine, GetRand(100)).WillOnce(Return(1));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 191);

  auto combined_res = target.GetResistances();
  int total_dr = 0;
  for (const auto& dr : combined_res.damage_reductions) {
    total_dr += dr.amount;
  }
  EXPECT_EQ(total_dr, 4);
}

TEST_F(MultiAbilityInteractionTest,
       DuroDeMatarNegatesThenRigidezRaivosaOnSecondHit) {
  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 2);
  auto target_cfg =
      MakeEntityConfig("Target", 10, 10, 0, MakeLongsword(),
                        {abilities::CreateDuroDeMatar(1),
                         abilities::CreateRigidezRaivosa()});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));

    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(3));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_TRUE(target.IsAlive());
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::DuroDeMatar), 0);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 2);
}

}  // namespace testing
}  // namespace ettes

#include <gtest/gtest.h>

#include <memory>

#include "src/internal/abilities/ability.h"
#include "src/internal/combat/combat_context.h"
#include "src/internal/combat/event_manager.h"
#include "src/internal/entities/entity.h"
#include "src/internal/rules/resistances.h"
#include "tests/integration/test_helpers.h"

using ::testing::Return;

namespace ettes {
namespace testing {

class RigidezRaivosaTest : public ::testing::Test {
 protected:
  void SetUp() override {
    harness_ = std::make_unique<TestHarness>();
  }

  std::unique_ptr<TestHarness> harness_;
};

TEST_F(RigidezRaivosaTest, GainsDROnEachHitTaken) {
  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 3);
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

  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::RigidezRaivosa), 3);
  auto combined_res = target.GetResistances();
  int total_bonus_dr = 0;
  for (const auto& dr :
       target.GetCurrentStats()
           .bonus_stats.bonus_resistances.damage_reductions) {
    total_bonus_dr += dr.amount;
  }
  EXPECT_EQ(total_bonus_dr, 3);
}

TEST_F(RigidezRaivosaTest, StackingDRReducesSubsequentDamage) {
  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 3);
  auto target_cfg = MakeEntityConfig("Target", 200, 10, 0, MakeLongsword(),
                                     {abilities::CreateRigidezRaivosa()});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(5));

    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(5));

    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(5));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 173);
}

TEST_F(RigidezRaivosaTest, HealResetsAllBonusDR) {
  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 3);
  auto target_cfg = MakeEntityConfig("Target", 200, 10, 0, MakeLongsword(),
                                     {abilities::CreateRigidezRaivosa()});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    for (int i = 0; i < 3; ++i) {
      EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
      EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(5));
    }
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::RigidezRaivosa), 3);

  combat::CombatEventContext heal_ctx;
  heal_ctx.target = &target;
  heal_ctx.is_temp_hp = false;
  heal_ctx.heal_amount = 10;
  combat::EmitCombatEvent(combat::CombatEvent::Heal, &heal_ctx);

  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::RigidezRaivosa), 0);

  int total_bonus_dr = 0;
  for (const auto& dr :
       target.GetCurrentStats()
           .bonus_stats.bonus_resistances.damage_reductions) {
    total_bonus_dr += dr.amount;
  }
  EXPECT_EQ(total_bonus_dr, 0);
}

TEST_F(RigidezRaivosaTest, DRAccumulatesAlongsideBaseDR) {
  entities::Resistances res;
  res.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 3});

  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 2);
  auto target_cfg =
      MakeEntityConfig("Target", 200, 10, 0, MakeLongsword(),
                        {abilities::CreateRigidezRaivosa()}, res);
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

  auto combined_res = target.GetResistances();
  int total_dr = 0;
  for (const auto& dr : combined_res.damage_reductions) {
    total_dr += dr.amount;
  }
  EXPECT_EQ(total_dr, 5);
}

TEST_F(RigidezRaivosaTest, PostHealDamageNoLongerReduced) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 200, 10, 0, MakeLongsword(),
                                     {abilities::CreateRigidezRaivosa()});
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(5));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 190);
  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::RigidezRaivosa), 1);

  combat::CombatEventContext heal_ctx;
  heal_ctx.target = &target;
  heal_ctx.is_temp_hp = false;
  heal_ctx.heal_amount = 5;
  combat::EmitCombatEvent(combat::CombatEvent::Heal, &heal_ctx);
  target.Heal(5);

  EXPECT_EQ(target.GetAbilityStack(abilities::AbilityId::RigidezRaivosa), 0);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(5));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 185);
}

}  // namespace testing
}  // namespace ettes

#include <gtest/gtest.h>

#include <memory>

#include "src/internal/combat/attack_result.h"
#include "src/internal/engine/combat_engine.h"
#include "src/internal/entities/entity.h"
#include "src/internal/items/enchantment_library.h"
#include "src/internal/rules/damage_types.h"
#include "tests/integration/test_helpers.h"

using ::testing::Return;

namespace ettes {
namespace testing {

class CombatPipelineTest : public ::testing::Test {
 protected:
  void SetUp() override {
    harness_ = std::make_unique<TestHarness>();
  }

  std::unique_ptr<TestHarness> harness_;
};

TEST_F(CombatPipelineTest, NormalHitDealsBaseDamage) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 50, 10, 0, MakeLongsword());
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 37);
}

TEST_F(CombatPipelineTest, MissDealsNoDamage) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 50, 15, 0, MakeLongsword());
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(1));

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 50);
}

TEST_F(CombatPipelineTest, CriticalHitMultipliesDamage) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 100, 10, 0, MakeLongsword());
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(20));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 74);
}

TEST_F(CombatPipelineTest, FullFortificationNegatesCritMultiplier) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg =
      MakeEntityConfig("Target", 100, 10, 100, MakeLongsword());
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

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 87);
}

TEST_F(CombatPipelineTest, PartialFortificationCritNegated) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 100, 10, 50, MakeLongsword());
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(20));
    EXPECT_CALL(*harness_->mock_engine, GetRand(100)).WillOnce(Return(25));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 87);
}

TEST_F(CombatPipelineTest, PartialFortificationCritGoesThrough) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 100, 10, 50, MakeLongsword());
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(20));
    EXPECT_CALL(*harness_->mock_engine, GetRand(100)).WillOnce(Return(75));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 74);
}

TEST_F(CombatPipelineTest, DamageReductionReducesPhysicalDamage) {
  entities::Resistances res;
  res.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 5});

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg =
      MakeEntityConfig("Target", 50, 10, 0, MakeLongsword(), {}, res);
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 42);
}

TEST_F(CombatPipelineTest, EnergyResistanceReducesEnergyDamage) {
  items::Weapon fire_sword = MakeLongsword();
  fire_sword.enchantments.push_back(items::CreateFlamingEnchantment());

  entities::Resistances res;
  res.energy_resistances.push_back({rules::DamageType::Fire, 10});

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, fire_sword);
  auto target_cfg =
      MakeEntityConfig("Target", 50, 10, 0, MakeLongsword(), {}, res);
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
    EXPECT_CALL(*harness_->mock_engine, GetRand(6)).WillOnce(Return(6));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 37);
}

TEST_F(CombatPipelineTest, ImmunityNegatesDamageTypeEntirely) {
  entities::Resistances res;
  res.immunity.immune_types = rules::DamageType::Slash;

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword());
  auto target_cfg =
      MakeEntityConfig("Target", 50, 10, 0, MakeLongsword(), {}, res);
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 50);
}

TEST_F(CombatPipelineTest, WeaknessIncreasesDamage) {
  items::Weapon fire_sword = MakeLongsword();
  fire_sword.enchantments.push_back(items::CreateFlamingEnchantment());

  entities::Resistances res;
  res.weaknesses.push_back({rules::DamageType::Fire, 5});

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, fire_sword);
  auto target_cfg =
      MakeEntityConfig("Target", 100, 10, 0, MakeLongsword(), {}, res);
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
    EXPECT_CALL(*harness_->mock_engine, GetRand(6)).WillOnce(Return(6));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 76);
}

}  // namespace testing
}  // namespace ettes

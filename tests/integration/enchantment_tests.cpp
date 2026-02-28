#include <gtest/gtest.h>

#include <memory>

#include "src/internal/entities/entity.h"
#include "src/internal/items/enchantment_library.h"
#include "src/internal/rules/alignment.h"
#include "src/internal/rules/damage_types.h"
#include "tests/integration/test_helpers.h"

using ::testing::Return;

namespace ettes {
namespace testing {

class EnchantmentIntegrationTest : public ::testing::Test {
 protected:
  void SetUp() override {
    harness_ = std::make_unique<TestHarness>();
  }

  std::unique_ptr<TestHarness> harness_;
};

TEST_F(EnchantmentIntegrationTest, FlamingAddsFireDamageOnHit) {
  items::Weapon fire_sword = MakeLongsword();
  fire_sword.enchantments.push_back(items::CreateFlamingEnchantment());

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, fire_sword);
  auto target_cfg = MakeEntityConfig("Target", 50, 10, 0, MakeLongsword());
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

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 31);
}

TEST_F(EnchantmentIntegrationTest, FlamingExplosionExtraDiceOnCrit) {
  items::Weapon explosion_sword = MakeLongsword();
  explosion_sword.enchantments.push_back(
      items::CreateFlamingExplosionEnchantment());

  auto attacker_cfg =
      MakeEntityConfig("Attacker", 50, 10, 0, explosion_sword);
  auto target_cfg = MakeEntityConfig("Target", 200, 10, 0, MakeLongsword());
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(20));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
    EXPECT_CALL(*harness_->mock_engine, GetRand(10)).WillOnce(Return(10));
    EXPECT_CALL(*harness_->mock_engine, GetRand(6)).WillOnce(Return(6));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 158);
}

TEST_F(EnchantmentIntegrationTest, FlamingExplosionNormalHitOnlyBaseFireDice) {
  items::Weapon explosion_sword = MakeLongsword();
  explosion_sword.enchantments.push_back(
      items::CreateFlamingExplosionEnchantment());

  auto attacker_cfg =
      MakeEntityConfig("Attacker", 50, 10, 0, explosion_sword);
  auto target_cfg = MakeEntityConfig("Target", 200, 10, 0, MakeLongsword());
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

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 181);
}

TEST_F(EnchantmentIntegrationTest, DissonantDealsDamageToTargetAndSelf) {
  items::Weapon dissonant_sword = MakeLongsword();
  dissonant_sword.enchantments.push_back(items::CreateDissonantEnchantment());

  auto attacker_cfg =
      MakeEntityConfig("Attacker", 50, 10, 0, dissonant_sword);
  auto target_cfg = MakeEntityConfig("Target", 100, 10, 0, MakeLongsword());
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(6)).WillOnce(Return(6));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
    EXPECT_CALL(*harness_->mock_engine, GetRand(6)).WillOnce(Return(6));
    EXPECT_CALL(*harness_->mock_engine, GetRand(6)).WillOnce(Return(6));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 75);
  EXPECT_EQ(attacker.GetCurrentStats().base_stats.hp, 44);
}

TEST_F(EnchantmentIntegrationTest, VampiricGrantsTempHP) {
  items::Weapon vamp_sword = MakeLongsword();
  vamp_sword.enchantments.push_back(items::CreateVampiricEnchantment());

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, vamp_sword);
  auto target_cfg = MakeEntityConfig("Target", 100, 10, 0, MakeLongsword());
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 87);
  EXPECT_EQ(attacker.GetCurrentStats().bonus_stats.temporary_hp, 6);
}

TEST_F(EnchantmentIntegrationTest, DrainingHealsAttackerOnHit) {
  items::Weapon drain_sword = MakeLongsword();
  drain_sword.enchantments.push_back(items::CreateDrainingEnchantment());

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, drain_sword);
  auto target_cfg = MakeEntityConfig("Target", 100, 10, 0, MakeLongsword());
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  attacker.TakeDamage(5);
  EXPECT_EQ(attacker.GetCurrentStats().base_stats.hp, 45);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(attacker.GetCurrentStats().base_stats.hp, 46);
}

TEST_F(EnchantmentIntegrationTest, ProfaneExtraDamageVsGoodTarget) {
  items::Weapon profane_sword = MakeLongsword();
  profane_sword.enchantments.push_back(items::CreateProfaneEnchantment());

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, profane_sword);
  auto target_cfg =
      MakeEntityConfig("Target", 100, 10, 0, MakeLongsword(), {}, {},
                        rules::Alignment::LawfulGood);
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
    EXPECT_CALL(*harness_->mock_engine, GetRand(6)).WillOnce(Return(6));
    EXPECT_CALL(*harness_->mock_engine, GetRand(6)).WillOnce(Return(6));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 75);
}

TEST_F(EnchantmentIntegrationTest, ProfaneNoBonusVsNonGoodTarget) {
  items::Weapon profane_sword = MakeLongsword();
  profane_sword.enchantments.push_back(items::CreateProfaneEnchantment());

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, profane_sword);
  auto target_cfg = MakeEntityConfig("Target", 100, 10, 0, MakeLongsword());
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(8));
  }

  harness_->engine->QueueAttack({&attacker, &target, 0});
  harness_->engine->Flush(nullptr);

  EXPECT_EQ(target.GetCurrentStats().base_stats.hp, 87);
}

}  // namespace testing
}  // namespace ettes

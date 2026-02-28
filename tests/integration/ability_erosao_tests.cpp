#include <gtest/gtest.h>

#include <memory>

#include "src/internal/abilities/ability.h"
#include "src/internal/entities/entity.h"
#include "src/internal/rules/resistances.h"
#include "tests/integration/test_helpers.h"

using ::testing::Return;

namespace ettes {
namespace testing {

class ErosaoTest : public ::testing::Test {
 protected:
  void SetUp() override {
    harness_ = std::make_unique<TestHarness>();
  }

  std::unique_ptr<TestHarness> harness_;
};

TEST_F(ErosaoTest, StripsBonusDROnHit) {
  entities::Resistances res;
  res.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 5});

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword(),
                                       {abilities::CreateErosao()});
  auto target_cfg =
      MakeEntityConfig("Target", 100, 10, 0, MakeLongsword(), {}, res);
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  target.AddDR({rules::DamageType::None, rules::DamageModifier::None, 3},
               true);

  {
    ::testing::InSequence seq;
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
  EXPECT_EQ(total_dr, 7);
}

TEST_F(ErosaoTest, StripsBaseDRAfterBonusIsGone) {
  entities::Resistances res;
  res.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 5});

  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword(),
                                       {abilities::CreateErosao()});
  auto target_cfg =
      MakeEntityConfig("Target", 100, 10, 0, MakeLongsword(), {}, res);
  entities::Entity attacker(attacker_cfg);
  entities::Entity target(target_cfg);

  {
    ::testing::InSequence seq;
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
  EXPECT_EQ(total_dr, 4);
}

TEST_F(ErosaoTest, MultipleHitsErodeMultipleDR) {
  entities::Resistances res;
  res.damage_reductions.push_back(
      {rules::DamageType::None, rules::DamageModifier::None, 5});

  auto attacker_cfg =
      MakeMultiAttackEntityConfig("Attacker", 50, 10, 0, MakeLongsword(), 3,
                                  {abilities::CreateErosao()});
  auto target_cfg =
      MakeEntityConfig("Target", 200, 10, 0, MakeLongsword(), {}, res);
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

  auto combined_res = target.GetResistances();
  int total_dr = 0;
  for (const auto& dr : combined_res.damage_reductions) {
    total_dr += dr.amount;
  }
  EXPECT_EQ(total_dr, 2);
}

TEST_F(ErosaoTest, NoDRToStripNoCrash) {
  auto attacker_cfg = MakeEntityConfig("Attacker", 50, 10, 0, MakeLongsword(),
                                       {abilities::CreateErosao()});
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

TEST_F(ErosaoTest, ErosionAffectsSubsequentDamageInSameRound) {
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

}  // namespace testing
}  // namespace ettes

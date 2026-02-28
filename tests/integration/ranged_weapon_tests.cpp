#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "src/internal/combat/attack_result.h"
#include "src/internal/engine/combat_engine.h"
#include "src/internal/engine/director.h"
#include "src/internal/engine/encounter.h"
#include "src/internal/entities/entity.h"
#include "src/internal/positioning/grid_pos.h"
#include "tests/integration/test_helpers.h"

using ::testing::Return;

namespace ettes {
namespace testing {

inline items::Weapon MakeLongbow() {
  return items::Weapon{
      .name = "Longbow",
      .attack_bonus = 0,
      .damage = dice_rolls::Term{.dice_groups = {{1, 8}}, .bonus = 3},
      .damage_type = rules::DamageType::Pierce,
      .damage_modifier = rules::DamageModifier::None,
      .crit_range = 20,
      .crit_multiplier = 3,
      .range_meters = 30.0,
      .enchantments = {}};
}

class RangedWeaponTest : public ::testing::Test {
 protected:
  void SetUp() override {
    harness_ = std::make_unique<TestHarness>();
  }

  std::unique_ptr<TestHarness> harness_;
};

TEST_F(RangedWeaponTest, RangedAttackerCanAttackFromDistance) {
  auto attacker_cfg = MakeEntityConfig("Archer", 50, 10, 0, MakeLongbow());
  auto target_cfg = MakeEntityConfig("Target", 50, 10, 0, MakeLongsword());

  auto attacker_ptr = std::make_unique<entities::Entity>(attacker_cfg);
  auto target_ptr = std::make_unique<entities::Entity>(target_cfg);

  entities::Entity* attacker_raw = attacker_ptr.get();
  entities::Entity* target_raw = target_ptr.get();

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(attacker_ptr));
  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(target_ptr));

  engine::Encounter encounter(std::move(side_a), std::move(side_b));
  engine::Director director(&encounter, harness_->engine.get());

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(6));
  }

  director.RunTurn(attacker_raw);

  EXPECT_EQ(target_raw->GetCurrentStats().base_stats.hp, 41);
}

TEST_F(RangedWeaponTest, RangedAttackerDoesNotMoveWhenInRange) {
  auto attacker_cfg = MakeEntityConfig("Archer", 50, 10, 0, MakeLongbow());
  auto target_cfg = MakeEntityConfig("Target", 50, 10, 0, MakeLongsword());

  auto attacker_ptr = std::make_unique<entities::Entity>(attacker_cfg);
  auto target_ptr = std::make_unique<entities::Entity>(target_cfg);

  entities::Entity* attacker_raw = attacker_ptr.get();

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(attacker_ptr));
  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(target_ptr));

  engine::Encounter encounter(std::move(side_a), std::move(side_b));
  engine::Director director(&encounter, harness_->engine.get());

  auto pos_before = director.GetPositionMap()->GetAnchor(attacker_raw);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(6));
  }

  director.RunTurn(attacker_raw);

  auto pos_after = director.GetPositionMap()->GetAnchor(attacker_raw);
  EXPECT_EQ(pos_before.x, pos_after.x);
  EXPECT_EQ(pos_before.y, pos_after.y);
}

TEST_F(RangedWeaponTest, MeleeAttackerMovesTowardTarget) {
  auto attacker_cfg = MakeEntityConfig("Fighter", 50, 10, 0, MakeLongsword());
  auto target_cfg = MakeEntityConfig("Target", 50, 10, 0, MakeLongsword());

  auto attacker_ptr = std::make_unique<entities::Entity>(attacker_cfg);
  auto target_ptr = std::make_unique<entities::Entity>(target_cfg);

  entities::Entity* attacker_raw = attacker_ptr.get();
  entities::Entity* target_raw = target_ptr.get();

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(attacker_ptr));
  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(target_ptr));

  engine::Encounter encounter(std::move(side_a), std::move(side_b));
  engine::Director director(&encounter, harness_->engine.get());

  auto pos_before = director.GetPositionMap()->GetAnchor(attacker_raw);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(8)).WillOnce(Return(6));
  }

  director.RunTurn(attacker_raw);

  auto pos_after = director.GetPositionMap()->GetAnchor(attacker_raw);
  EXPECT_NE(pos_before.y, pos_after.y);

  EXPECT_TRUE(director.GetPositionMap()->AreAdjacent(attacker_raw, target_raw));
  EXPECT_LT(target_raw->GetCurrentStats().base_stats.hp, 50);
}

TEST_F(RangedWeaponTest, OutOfRangeRangedAttackerMovesCloser) {
  auto short_bow = items::Weapon{
      .name = "ShortBow",
      .attack_bonus = 0,
      .damage = dice_rolls::Term{.dice_groups = {{1, 6}}, .bonus = 2},
      .damage_type = rules::DamageType::Pierce,
      .damage_modifier = rules::DamageModifier::None,
      .crit_range = 20,
      .crit_multiplier = 3,
      .range_meters = 4.5,
      .enchantments = {}};

  auto attacker_cfg = MakeEntityConfig("Archer", 50, 10, 0, short_bow);
  auto target_cfg = MakeEntityConfig("Target", 50, 10, 0, MakeLongsword());

  auto attacker_ptr = std::make_unique<entities::Entity>(attacker_cfg);
  auto target_ptr = std::make_unique<entities::Entity>(target_cfg);

  entities::Entity* attacker_raw = attacker_ptr.get();
  entities::Entity* target_raw = target_ptr.get();

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(attacker_ptr));
  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(target_ptr));

  engine::Encounter encounter(std::move(side_a), std::move(side_b));
  engine::Director director(&encounter, harness_->engine.get());

  auto pos_before = director.GetPositionMap()->GetAnchor(attacker_raw);
  double dist_before =
      director.GetPositionMap()->GetDistanceMeters(attacker_raw, target_raw);
  EXPECT_GT(dist_before, 4.5);

  {
    ::testing::InSequence seq;
    EXPECT_CALL(*harness_->mock_engine, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*harness_->mock_engine, GetRand(6)).WillOnce(Return(4));
  }

  director.RunTurn(attacker_raw);

  auto pos_after = director.GetPositionMap()->GetAnchor(attacker_raw);
  EXPECT_NE(pos_before.y, pos_after.y);

  double dist_after =
      director.GetPositionMap()->GetDistanceMeters(attacker_raw, target_raw);
  EXPECT_LE(dist_after, 4.5);
  EXPECT_LT(target_raw->GetCurrentStats().base_stats.hp, 50);
}

}  // namespace testing
}  // namespace ettes

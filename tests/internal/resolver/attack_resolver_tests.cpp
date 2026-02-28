#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "internal/combat/attack.h"
#include "internal/combat/combat_context.h"
#include "internal/dice_rolls/roller.h"
#include "internal/items/enchantment.h"
#include "internal/resolver/attack_resolver.h"
#include "internal/rules/damage_types.h"
#include "tests/mocks/mock_entity.h"
#include "tests/mocks/mock_random_engine.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;

namespace ettes {
namespace resolver {

class AttackResolverTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_engine_ = std::make_shared<dice_rolls::MockRandomEngine>();
    roller_ = std::make_shared<dice_rolls::Roller>(mock_engine_);
    source_ = std::make_unique<MockEntity>();
    target_ = std::make_unique<MockEntity>();

    ON_CALL(*source_, GetName()).WillByDefault(ReturnRef(source_name_));
    ON_CALL(*target_, GetName()).WillByDefault(ReturnRef(target_name_));
    ON_CALL(*source_, GetCurrentStats())
        .WillByDefault(ReturnRef(source_stats_));
    ON_CALL(*target_, GetCurrentStats())
        .WillByDefault(ReturnRef(target_stats_));
    ON_CALL(*source_, GetEffectsForEvent(_))
        .WillByDefault(ReturnRef(empty_effects_));
    ON_CALL(*target_, GetEffectsForEvent(_))
        .WillByDefault(ReturnRef(empty_effects_));

    context_.source = source_.get();
    context_.target = target_.get();
    context_.roller = roller_.get();
  }

  combat::AttackMove MakeAttackMove() {
    combat::AttackMove move;
    move.weapon.name = "Longsword";
    move.weapon.attack_bonus = 0;
    move.weapon.damage =
        dice_rolls::Term{.dice_groups = {{1, 8}}, .bonus = 5};
    move.weapon.damage_type = rules::DamageType::Slash;
    move.weapon.damage_modifier = rules::DamageModifier::None;
    move.weapon.crit_range = 19;
    move.weapon.crit_multiplier = 2;
    move.attack_modifier = 0;
    move.damage_modifier = 0;
    move.crit_range_bonus = 0;
    move.crit_multiplier_bonus = 0;
    return move;
  }

  combat::AttackSequence MakeSequence() {
    combat::AttackSequence seq;
    seq.name = "Default";
    seq.attacks.push_back(MakeAttackMove());
    seq.attack_modifier = 0;
    seq.damage_modifier = 0;
    return seq;
  }

  std::string source_name_ = "Attacker";
  std::string target_name_ = "Target";
  entities::Stats source_stats_;
  entities::Stats target_stats_;
  std::shared_ptr<dice_rolls::MockRandomEngine> mock_engine_;
  std::shared_ptr<dice_rolls::Roller> roller_;
  std::unique_ptr<MockEntity> source_;
  std::unique_ptr<MockEntity> target_;
  combat::CombatEventContext context_;
  std::vector<const combat::Effect*> empty_effects_;
};

TEST_F(AttackResolverTest, OutOfBoundsIndexDoesNothing) {
  auto seq = MakeSequence();
  AttackResolver resolver(seq, &context_);

  resolver.ResolveSingleAttack(99);

  EXPECT_TRUE(context_.results.empty());
}

TEST_F(AttackResolverTest, MissDoesNotGenerateDamage) {
  auto seq = MakeSequence();
  AttackResolver resolver(seq, &context_);

  ON_CALL(*target_, GetEffectiveAC()).WillByDefault(Return(30));
  ON_CALL(*target_, GetFortification()).WillByDefault(Return(0));
  EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(5));

  resolver.ResolveSingleAttack(0);

  ASSERT_EQ(context_.results.size(), 1u);
  EXPECT_FALSE(context_.results[0].is_hit);
  EXPECT_TRUE(context_.results[0].damage_instances.empty());
}

TEST_F(AttackResolverTest, HitGeneratesBaseDamage) {
  auto seq = MakeSequence();
  AttackResolver resolver(seq, &context_);

  ON_CALL(*target_, GetEffectiveAC()).WillByDefault(Return(10));
  ON_CALL(*target_, GetFortification()).WillByDefault(Return(0));

  {
    ::testing::InSequence s;
    EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*mock_engine_, GetRand(8)).WillOnce(Return(6));
  }

  resolver.ResolveSingleAttack(0);

  ASSERT_EQ(context_.results.size(), 1u);
  EXPECT_TRUE(context_.results[0].is_hit);
  EXPECT_FALSE(context_.results[0].is_crit);
  EXPECT_EQ(context_.results[0].crit_multiplier, 1);
  ASSERT_EQ(context_.results[0].damage_instances.size(), 1u);
  EXPECT_EQ(context_.results[0].damage_instances[0].amount, 11);
}

TEST_F(AttackResolverTest, CriticalHitMultipliesDamage) {
  auto seq = MakeSequence();
  AttackResolver resolver(seq, &context_);

  ON_CALL(*target_, GetEffectiveAC()).WillByDefault(Return(10));
  ON_CALL(*target_, GetFortification()).WillByDefault(Return(0));

  {
    ::testing::InSequence s;
    EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(19));
    EXPECT_CALL(*mock_engine_, GetRand(8)).WillOnce(Return(5));
  }

  resolver.ResolveSingleAttack(0);

  ASSERT_EQ(context_.results.size(), 1u);
  EXPECT_TRUE(context_.results[0].is_crit);
  EXPECT_EQ(context_.results[0].crit_multiplier, 2);
  EXPECT_EQ(context_.results[0].damage_instances[0].amount, 20);
}

TEST_F(AttackResolverTest, CritNegatedByFullFortification) {
  auto seq = MakeSequence();
  AttackResolver resolver(seq, &context_);

  ON_CALL(*target_, GetEffectiveAC()).WillByDefault(Return(10));
  ON_CALL(*target_, GetFortification()).WillByDefault(Return(100));

  {
    ::testing::InSequence s;
    EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(20));
    EXPECT_CALL(*mock_engine_, GetRand(8)).WillOnce(Return(5));
  }

  resolver.ResolveSingleAttack(0);

  ASSERT_EQ(context_.results.size(), 1u);
  EXPECT_TRUE(context_.results[0].is_hit);
  EXPECT_FALSE(context_.results[0].is_crit);
  EXPECT_EQ(context_.results[0].crit_multiplier, 1);
}

TEST_F(AttackResolverTest, CritNegatedByPartialFortificationRoll) {
  auto seq = MakeSequence();
  AttackResolver resolver(seq, &context_);

  ON_CALL(*target_, GetEffectiveAC()).WillByDefault(Return(10));
  ON_CALL(*target_, GetFortification()).WillByDefault(Return(50));

  {
    ::testing::InSequence s;
    EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(20));
    EXPECT_CALL(*mock_engine_, GetRand(100)).WillOnce(Return(30));
    EXPECT_CALL(*mock_engine_, GetRand(8)).WillOnce(Return(5));
  }

  resolver.ResolveSingleAttack(0);

  ASSERT_EQ(context_.results.size(), 1u);
  EXPECT_TRUE(context_.results[0].is_hit);
  EXPECT_FALSE(context_.results[0].is_crit);
  EXPECT_EQ(context_.results[0].crit_multiplier, 1);
}

TEST_F(AttackResolverTest, CritSurvivesPartialFortificationRoll) {
  auto seq = MakeSequence();
  AttackResolver resolver(seq, &context_);

  ON_CALL(*target_, GetEffectiveAC()).WillByDefault(Return(10));
  ON_CALL(*target_, GetFortification()).WillByDefault(Return(50));

  {
    ::testing::InSequence s;
    EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(20));
    EXPECT_CALL(*mock_engine_, GetRand(100)).WillOnce(Return(80));
    EXPECT_CALL(*mock_engine_, GetRand(8)).WillOnce(Return(5));
  }

  resolver.ResolveSingleAttack(0);

  ASSERT_EQ(context_.results.size(), 1u);
  EXPECT_TRUE(context_.results[0].is_crit);
  EXPECT_EQ(context_.results[0].crit_multiplier, 2);
}

TEST_F(AttackResolverTest, NegativeDamageClampedToOne) {
  combat::AttackMove move = MakeAttackMove();
  move.weapon.damage =
      dice_rolls::Term{.dice_groups = {{1, 4}}, .bonus = -10};
  combat::AttackSequence seq;
  seq.name = "Default";
  seq.attacks.push_back(move);
  seq.attack_modifier = 0;
  seq.damage_modifier = 0;

  AttackResolver resolver(seq, &context_);

  ON_CALL(*target_, GetEffectiveAC()).WillByDefault(Return(10));
  ON_CALL(*target_, GetFortification()).WillByDefault(Return(0));

  {
    ::testing::InSequence s;
    EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*mock_engine_, GetRand(4)).WillOnce(Return(1));
  }

  resolver.ResolveSingleAttack(0);

  ASSERT_EQ(context_.results.size(), 1u);
  EXPECT_EQ(context_.results[0].damage_instances[0].amount, 1);
}

TEST_F(AttackResolverTest, EnchantmentZeroDamageNotAdded) {
  combat::AttackMove move = MakeAttackMove();
  items::Enchantment ench;
  ench.name = "ZeroEnchant";
  items::DamageSource dsource;
  dsource.name = "Zero";
  dsource.contribute = [](const combat::CombatEventContext&) {
    return rules::DamageInstance{.amount = 0, .types = 0, .modifiers = 0};
  };
  ench.damage_sources.push_back(dsource);
  move.weapon.enchantments.push_back(ench);

  combat::AttackSequence seq;
  seq.name = "Default";
  seq.attacks.push_back(move);
  seq.attack_modifier = 0;
  seq.damage_modifier = 0;

  AttackResolver resolver(seq, &context_);

  ON_CALL(*target_, GetEffectiveAC()).WillByDefault(Return(10));
  ON_CALL(*target_, GetFortification()).WillByDefault(Return(0));

  {
    ::testing::InSequence s;
    EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*mock_engine_, GetRand(8)).WillOnce(Return(6));
  }

  resolver.ResolveSingleAttack(0);

  ASSERT_EQ(context_.results.size(), 1u);
  EXPECT_EQ(context_.results[0].damage_instances.size(), 1u);
}

TEST_F(AttackResolverTest, MultipleAttacksInSequence) {
  combat::AttackSequence seq = MakeSequence();
  seq.attacks.push_back(MakeAttackMove());
  AttackResolver resolver(seq, &context_);

  ON_CALL(*target_, GetEffectiveAC()).WillByDefault(Return(10));
  ON_CALL(*target_, GetFortification()).WillByDefault(Return(0));

  {
    ::testing::InSequence s;
    EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*mock_engine_, GetRand(8)).WillOnce(Return(4));
    EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(15));
    EXPECT_CALL(*mock_engine_, GetRand(8)).WillOnce(Return(6));
  }

  resolver.ResolveSingleAttack(0);
  resolver.ResolveSingleAttack(1);

  ASSERT_EQ(context_.results.size(), 2u);
  EXPECT_TRUE(context_.results[0].is_hit);
  EXPECT_TRUE(context_.results[1].is_hit);
}

TEST_F(AttackResolverTest, NatTwentyCritWithHighMultiplier) {
  combat::AttackMove move = MakeAttackMove();
  move.weapon.crit_range = 20;
  move.weapon.crit_multiplier = 3;
  combat::AttackSequence seq;
  seq.name = "Default";
  seq.attacks.push_back(move);
  seq.attack_modifier = 0;
  seq.damage_modifier = 0;

  AttackResolver resolver(seq, &context_);

  ON_CALL(*target_, GetEffectiveAC()).WillByDefault(Return(10));
  ON_CALL(*target_, GetFortification()).WillByDefault(Return(0));

  {
    ::testing::InSequence s;
    EXPECT_CALL(*mock_engine_, GetRand(20)).WillOnce(Return(20));
    EXPECT_CALL(*mock_engine_, GetRand(8)).WillOnce(Return(3));
  }

  resolver.ResolveSingleAttack(0);

  ASSERT_EQ(context_.results.size(), 1u);
  EXPECT_TRUE(context_.results[0].is_crit);
  EXPECT_EQ(context_.results[0].crit_multiplier, 3);
  EXPECT_EQ(context_.results[0].damage_instances[0].amount, 24);
}

}  // namespace resolver
}  // namespace ettes

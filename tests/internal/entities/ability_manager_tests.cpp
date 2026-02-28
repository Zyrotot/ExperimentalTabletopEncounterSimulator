// -----------------------------------------------------------------------------
// | @file      ability_manager_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/entities/ability_manager.h"

namespace ettes {
namespace entities {

class AbilityManagerTest : public ::testing::Test {
 protected:
  abilities::Ability MakeAbility(const std::string& name,
                                 abilities::AbilityId id, int stacks = 0,
                                 bool active = true) {
    abilities::Ability ability;
    ability.name = name;
    ability.id = id;
    ability.stack_count = stacks;
    ability.is_active = active;
    return ability;
  }
};

TEST_F(AbilityManagerTest, HasAbilityReturnsTrueWhenPresent) {
  AbilityManager mgr({MakeAbility("Erosao", abilities::AbilityId::Erosao)});

  EXPECT_TRUE(mgr.HasAbility(abilities::AbilityId::Erosao));
}

TEST_F(AbilityManagerTest, HasAbilityReturnsFalseWhenAbsent) {
  AbilityManager mgr({MakeAbility("Erosao", abilities::AbilityId::Erosao)});

  EXPECT_FALSE(mgr.HasAbility(abilities::AbilityId::Trespassar));
}

TEST_F(AbilityManagerTest, GetAbilityStackReturnsValue) {
  AbilityManager mgr(
      {MakeAbility("Test", abilities::AbilityId::DuroDeFerir, 3)});

  EXPECT_EQ(mgr.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 3);
}

TEST_F(AbilityManagerTest, GetAbilityStackReturnsZeroForMissing) {
  AbilityManager mgr({});

  EXPECT_EQ(mgr.GetAbilityStack(abilities::AbilityId::Erosao), 0);
}

TEST_F(AbilityManagerTest, IncrementAbilityStackIncreases) {
  AbilityManager mgr(
      {MakeAbility("Test", abilities::AbilityId::DuroDeFerir, 2)});

  mgr.IncrementAbilityStack(abilities::AbilityId::DuroDeFerir);

  EXPECT_EQ(mgr.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 3);
}

TEST_F(AbilityManagerTest, DecrementAbilityStackDecreases) {
  AbilityManager mgr(
      {MakeAbility("Test", abilities::AbilityId::DuroDeFerir, 2)});

  mgr.DecrementAbilityStack(abilities::AbilityId::DuroDeFerir);

  EXPECT_EQ(mgr.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 1);
}

TEST_F(AbilityManagerTest, DecrementAbilityStackDoesNotGoBelowZero) {
  AbilityManager mgr(
      {MakeAbility("Test", abilities::AbilityId::DuroDeFerir, 0)});

  mgr.DecrementAbilityStack(abilities::AbilityId::DuroDeFerir);

  EXPECT_EQ(mgr.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 0);
}

TEST_F(AbilityManagerTest, SetAbilityStackSetsValue) {
  AbilityManager mgr(
      {MakeAbility("Test", abilities::AbilityId::DuroDeFerir, 0)});

  mgr.SetAbilityStack(abilities::AbilityId::DuroDeFerir, 10);

  EXPECT_EQ(mgr.GetAbilityStack(abilities::AbilityId::DuroDeFerir), 10);
}

TEST_F(AbilityManagerTest, BuildActiveEffectsCollectsFromActiveAbilities) {
  abilities::Ability ability =
      MakeAbility("Test", abilities::AbilityId::Erosao, 0, true);
  combat::Effect eff;
  eff.name = "test_effect";
  eff.trigger = combat::CombatEvent::Hit;
  eff.is_active = true;
  ability.effects.push_back(eff);

  AbilityManager mgr({ability});
  mgr.BuildActiveEffects();

  EXPECT_EQ(mgr.GetActiveEffects().size(), 1u);
  EXPECT_EQ(mgr.GetActiveEffects()[0]->name, "test_effect");
}

TEST_F(AbilityManagerTest, BuildActiveEffectsSkipsInactiveAbilities) {
  abilities::Ability ability =
      MakeAbility("Test", abilities::AbilityId::Erosao, 0, false);
  combat::Effect eff;
  eff.name = "test_effect";
  eff.trigger = combat::CombatEvent::Hit;
  eff.is_active = true;
  ability.effects.push_back(eff);

  AbilityManager mgr({ability});
  mgr.BuildActiveEffects();

  EXPECT_TRUE(mgr.GetActiveEffects().empty());
}

TEST_F(AbilityManagerTest, GetEffectsForEventReturnsBucketedEffects) {
  abilities::Ability ability =
      MakeAbility("Test", abilities::AbilityId::Erosao, 0, true);
  combat::Effect hit_eff;
  hit_eff.name = "on_hit";
  hit_eff.trigger = combat::CombatEvent::Hit;
  hit_eff.is_active = true;
  ability.effects.push_back(hit_eff);

  combat::Effect kill_eff;
  kill_eff.name = "on_kill";
  kill_eff.trigger = combat::CombatEvent::Kill;
  kill_eff.is_active = true;
  ability.effects.push_back(kill_eff);

  AbilityManager mgr({ability});
  mgr.BuildActiveEffects();

  EXPECT_EQ(mgr.GetEffectsForEvent(combat::CombatEvent::Hit).size(), 1u);
  EXPECT_EQ(mgr.GetEffectsForEvent(combat::CombatEvent::Kill).size(), 1u);
  EXPECT_TRUE(
      mgr.GetEffectsForEvent(combat::CombatEvent::TurnStart).empty());
}

TEST_F(AbilityManagerTest, GetAbilitiesReturnsAll) {
  AbilityManager mgr(
      {MakeAbility("A", abilities::AbilityId::Erosao),
       MakeAbility("B", abilities::AbilityId::Trespassar)});

  EXPECT_EQ(mgr.GetAbilities().size(), 2u);
}

TEST_F(AbilityManagerTest, IncrementMissingAbilityDoesNothing) {
  AbilityManager mgr({});
  mgr.IncrementAbilityStack(abilities::AbilityId::Erosao);
  EXPECT_EQ(mgr.GetAbilityStack(abilities::AbilityId::Erosao), 0);
}

TEST_F(AbilityManagerTest, DecrementMissingAbilityDoesNothing) {
  AbilityManager mgr({});
  mgr.DecrementAbilityStack(abilities::AbilityId::Erosao);
  EXPECT_EQ(mgr.GetAbilityStack(abilities::AbilityId::Erosao), 0);
}

TEST_F(AbilityManagerTest, SetStackMissingAbilityDoesNothing) {
  AbilityManager mgr({});
  mgr.SetAbilityStack(abilities::AbilityId::Erosao, 10);
  EXPECT_EQ(mgr.GetAbilityStack(abilities::AbilityId::Erosao), 0);
}

TEST_F(AbilityManagerTest, GetEffectsForOutOfBoundsEventReturnsEmpty) {
  AbilityManager mgr({});
  auto event = static_cast<combat::CombatEvent>(99);
  EXPECT_TRUE(mgr.GetEffectsForEvent(event).empty());
}

TEST_F(AbilityManagerTest, AbilityWithNoneIdNotInIndex) {
  AbilityManager mgr(
      {MakeAbility("NoneAbility", abilities::AbilityId::None)});
  EXPECT_FALSE(mgr.HasAbility(abilities::AbilityId::None));
  EXPECT_EQ(mgr.GetAbilities().size(), 1u);
}

}  // namespace entities
}  // namespace ettes

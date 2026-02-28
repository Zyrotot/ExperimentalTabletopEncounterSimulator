// -----------------------------------------------------------------------------
// | @file      event_manager_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "internal/combat/combat_context.h"
#include "internal/combat/event_manager.h"
#include "tests/mocks/mock_entity.h"

using ::testing::Return;
using ::testing::ReturnRef;

namespace ettes {
namespace combat {

class EventManagerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    source_ = std::make_unique<MockEntity>();
    target_ = std::make_unique<MockEntity>();
    ON_CALL(*source_, GetName()).WillByDefault(ReturnRef(source_name_));
    ON_CALL(*target_, GetName()).WillByDefault(ReturnRef(target_name_));
  }

  std::string source_name_ = "source";
  std::string target_name_ = "target";
  std::unique_ptr<MockEntity> source_;
  std::unique_ptr<MockEntity> target_;
};

TEST_F(EventManagerTest, NullContextReturnsEmptyModifications) {
  auto result = EmitCombatEvent(CombatEvent::Hit, nullptr);

  EXPECT_TRUE(result.empty());
}

TEST_F(EventManagerTest, EmitsEventToSourceOnHit) {
  bool was_called = false;
  Effect eff;
  eff.name = "test";
  eff.trigger = CombatEvent::Hit;
  eff.is_active = true;
  eff.on_event = [&](const CombatEventContext&) { was_called = true; };

  std::vector<const Effect*> effects = {&eff};

  ON_CALL(*source_, GetEffectsForEvent(CombatEvent::Hit))
      .WillByDefault(ReturnRef(effects));

  CombatEventContext context;
  context.source = source_.get();
  context.target = target_.get();

  EmitCombatEvent(CombatEvent::Hit, &context);

  EXPECT_TRUE(was_called);
}

TEST_F(EventManagerTest, EmitsEventToTargetOnTakeDamage) {
  bool was_called = false;
  Effect eff;
  eff.name = "test";
  eff.trigger = CombatEvent::TakeDamage;
  eff.is_active = true;
  eff.on_event = [&](const CombatEventContext&) { was_called = true; };

  std::vector<const Effect*> effects = {&eff};

  ON_CALL(*target_, GetEffectsForEvent(CombatEvent::TakeDamage))
      .WillByDefault(ReturnRef(effects));

  CombatEventContext context;
  context.source = source_.get();
  context.target = target_.get();

  EmitCombatEvent(CombatEvent::TakeDamage, &context);

  EXPECT_TRUE(was_called);
}

TEST_F(EventManagerTest, OnDamageReturnsDamageModification) {
  Effect eff;
  eff.name = "negate";
  eff.trigger = CombatEvent::TakeDamage;
  eff.is_active = true;
  eff.on_damage = [](const CombatEventContext&) {
    return DamageModification{.negate_all = true};
  };

  std::vector<const Effect*> effects = {&eff};

  ON_CALL(*target_, GetEffectsForEvent(CombatEvent::TakeDamage))
      .WillByDefault(ReturnRef(effects));

  CombatEventContext context;
  context.source = source_.get();
  context.target = target_.get();

  auto mods = EmitCombatEvent(CombatEvent::TakeDamage, &context);

  ASSERT_EQ(mods.size(), 1u);
  EXPECT_TRUE(mods[0].negate_all);
}

TEST_F(EventManagerTest, InactiveEffectsAreSkipped) {
  bool was_called = false;
  Effect eff;
  eff.name = "test";
  eff.trigger = CombatEvent::Hit;
  eff.is_active = false;
  eff.on_event = [&](const CombatEventContext&) { was_called = true; };

  std::vector<const Effect*> effects = {&eff};

  ON_CALL(*source_, GetEffectsForEvent(CombatEvent::Hit))
      .WillByDefault(ReturnRef(effects));

  CombatEventContext context;
  context.source = source_.get();
  context.target = target_.get();

  EmitCombatEvent(CombatEvent::Hit, &context);

  EXPECT_FALSE(was_called);
}

TEST_F(EventManagerTest, TransientEffectsAreTriggered) {
  bool was_called = false;
  Effect transient;
  transient.name = "enchant_hit";
  transient.trigger = CombatEvent::Hit;
  transient.is_active = true;
  transient.on_event = [&](const CombatEventContext&) { was_called = true; };

  std::vector<const Effect*> empty_effects;
  ON_CALL(*source_, GetEffectsForEvent(CombatEvent::Hit))
      .WillByDefault(ReturnRef(empty_effects));

  CombatEventContext context;
  context.source = source_.get();
  context.target = target_.get();
  context.transient_effects = {&transient};

  EmitCombatEvent(CombatEvent::Hit, &context);

  EXPECT_TRUE(was_called);
}

TEST_F(EventManagerTest, TransientEffectsWithWrongTriggerAreSkipped) {
  bool was_called = false;
  Effect transient;
  transient.name = "on_kill";
  transient.trigger = CombatEvent::Kill;
  transient.is_active = true;
  transient.on_event = [&](const CombatEventContext&) { was_called = true; };

  std::vector<const Effect*> empty_effects;
  ON_CALL(*source_, GetEffectsForEvent(CombatEvent::Hit))
      .WillByDefault(ReturnRef(empty_effects));

  CombatEventContext context;
  context.source = source_.get();
  context.target = target_.get();
  context.transient_effects = {&transient};

  EmitCombatEvent(CombatEvent::Hit, &context);

  EXPECT_FALSE(was_called);
}

}  // namespace combat
}  // namespace ettes

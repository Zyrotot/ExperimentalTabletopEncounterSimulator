// -----------------------------------------------------------------------------
// | @file      entity_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/entities/entity.h"

namespace ettes {
namespace entities {

class EntityTest : public ::testing::Test {
 protected:
  EntityConfig MakeConfig(int hp = 50, int ac = 15) {
    EntityConfig config;
    config.name = "TestEntity";
    config.starting_stats.base_stats.hp = hp;
    config.starting_stats.base_stats.armour_class = ac;
    config.starting_stats.base_stats.fortification = 0;
    config.alignment = rules::Alignment::TrueNeutral;

    items::Weapon weapon;
    weapon.name = "sword";
    weapon.attack_bonus = 5;
    weapon.damage = dice_rolls::Term{.dice_groups = {{1, 8}}, .bonus = 0};
    weapon.damage_type = rules::DamageType::Slash;
    weapon.crit_range = 19;
    weapon.crit_multiplier = 2;
    config.equipped_weapons = {weapon};

    combat::AttackSequence seq;
    seq.name = "primary";
    seq.attacks = {{weapon, 0, 0, 0, 0}};
    config.attack_sequences = {seq};

    return config;
  }
};

TEST_F(EntityTest, ConstructionSetsNameAndHP) {
  Entity entity(MakeConfig(50, 15));

  EXPECT_EQ(entity.GetName(), "TestEntity");
  EXPECT_EQ(entity.GetCurrentStats().base_stats.hp, 50);
  EXPECT_TRUE(entity.IsAlive());
}

TEST_F(EntityTest, TakeDamageReducesHP) {
  Entity entity(MakeConfig(50, 15));

  entity.TakeDamage(20);

  EXPECT_EQ(entity.GetCurrentStats().base_stats.hp, 30);
}

TEST_F(EntityTest, HealRestoresHP) {
  Entity entity(MakeConfig(50, 15));
  entity.TakeDamage(30);

  entity.Heal(15);

  EXPECT_EQ(entity.GetCurrentStats().base_stats.hp, 35);
}

TEST_F(EntityTest, HealDoesNotExceedStartingHP) {
  Entity entity(MakeConfig(50, 15));
  entity.TakeDamage(10);

  entity.Heal(999);

  EXPECT_EQ(entity.GetCurrentStats().base_stats.hp, 50);
}

TEST_F(EntityTest, GetEffectiveACReturnsCorrectValue) {
  Entity entity(MakeConfig(50, 15));

  EXPECT_EQ(entity.GetEffectiveAC(), 15);
}

TEST_F(EntityTest, GetAttackSequenceReturnsConfiguredSequence) {
  Entity entity(MakeConfig());

  EXPECT_EQ(entity.GetAttackSequence(0).name, "primary");
}

TEST_F(EntityTest, GetAlignmentReturnsConfiguredAlignment) {
  Entity entity(MakeConfig());

  EXPECT_EQ(entity.GetAlignment(), rules::Alignment::TrueNeutral);
}

TEST_F(EntityTest, AddAndRetrieveDR) {
  Entity entity(MakeConfig());
  rules::DamageReduction dr{rules::DamageType::None,
                            rules::DamageModifier::None, 5};
  entity.AddDR(dr);

  Resistances res = entity.GetResistances();

  EXPECT_EQ(res.damage_reductions.size(), 1u);
  EXPECT_EQ(res.damage_reductions[0].amount, 5);
}

TEST_F(EntityTest, ClearAllDRRemovesBonusDR) {
  Entity entity(MakeConfig());
  rules::DamageReduction dr{rules::DamageType::None,
                            rules::DamageModifier::None, 5};
  entity.AddDR(dr, true);
  entity.ClearAllDR(true);

  Resistances res = entity.GetResistances();

  EXPECT_TRUE(res.damage_reductions.empty());
}

TEST_F(EntityTest, UniqueIdsPerEntity) {
  Entity e1(MakeConfig());
  Entity e2(MakeConfig());

  EXPECT_NE(e1.GetId(), e2.GetId());
}

TEST_F(EntityTest, LethalDamageKillsEntity) {
  Entity entity(MakeConfig(50, 15));

  entity.TakeDamage(50);

  EXPECT_FALSE(entity.IsAlive());
}

TEST_F(EntityTest, AddTempHPAbsorbsDamageFirst) {
  Entity entity(MakeConfig(50, 15));
  entity.AddTempHP(20);

  entity.TakeDamage(15);

  EXPECT_EQ(entity.GetCurrentStats().bonus_stats.temporary_hp, 5);
  EXPECT_EQ(entity.GetCurrentStats().base_stats.hp, 50);
}

}  // namespace entities
}  // namespace ettes

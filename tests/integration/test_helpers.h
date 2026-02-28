#ifndef TESTS_INTEGRATION_TEST_HELPERS_H_
#define TESTS_INTEGRATION_TEST_HELPERS_H_

#include <memory>
#include <string>
#include <vector>

#include "src/internal/abilities/ability.h"
#include "src/internal/abilities/ability_id.h"
#include "src/internal/combat/attack.h"
#include "src/internal/dice_rolls/roller.h"
#include "src/internal/dice_rolls/term.h"
#include "src/internal/engine/combat_engine.h"
#include "src/internal/entities/entity.h"
#include "src/internal/entities/entity_config.h"
#include "src/internal/entities/stats.h"
#include "src/internal/items/weapon.h"
#include "src/internal/rules/alignment.h"
#include "src/internal/rules/damage_types.h"
#include "src/internal/rules/resistances.h"
#include "tests/mocks/mock_random_engine.h"

namespace ettes {
namespace testing {

inline items::Weapon MakeLongsword() {
  return items::Weapon{
      .name = "Longsword",
      .attack_bonus = 0,
      .damage = dice_rolls::Term{.dice_groups = {{1, 8}}, .bonus = 5},
      .damage_type = rules::DamageType::Slash,
      .damage_modifier = rules::DamageModifier::None,
      .crit_range = 19,
      .crit_multiplier = 2,
      .enchantments = {}};
}

inline items::Weapon MakeGreataxe() {
  return items::Weapon{
      .name = "Greataxe",
      .attack_bonus = 0,
      .damage = dice_rolls::Term{.dice_groups = {{1, 12}}, .bonus = 7},
      .damage_type = rules::DamageType::Slash,
      .damage_modifier = rules::DamageModifier::None,
      .crit_range = 20,
      .crit_multiplier = 3,
      .enchantments = {}};
}

inline items::Weapon MakeScimitar() {
  return items::Weapon{
      .name = "Scimitar",
      .attack_bonus = 0,
      .damage = dice_rolls::Term{.dice_groups = {{1, 6}}, .bonus = 3},
      .damage_type = rules::DamageType::Slash,
      .damage_modifier = rules::DamageModifier::None,
      .crit_range = 18,
      .crit_multiplier = 2,
      .enchantments = {}};
}

inline entities::EntityConfig MakeEntityConfig(
    const std::string& name, int hp, int ac, int fortification,
    items::Weapon weapon, std::vector<abilities::Ability> abilities = {},
    entities::Resistances resistances = {},
    rules::Alignment alignment = rules::Alignment::TrueNeutral) {
  combat::AttackMove move;
  move.weapon = weapon;
  move.attack_modifier = 0;
  move.damage_modifier = 0;
  move.crit_range_bonus = 0;
  move.crit_multiplier_bonus = 0;

  combat::AttackSequence sequence;
  sequence.name = "Default";
  sequence.attacks.push_back(move);
  sequence.attack_modifier = 0;
  sequence.damage_modifier = 0;

  entities::EntityConfig config;
  config.name = name;
  config.starting_stats.base_stats.hp = hp;
  config.starting_stats.base_stats.armour_class = ac;
  config.starting_stats.base_stats.fortification = fortification;
  config.starting_stats.base_stats.resistances = resistances;
  config.equipped_weapons = {weapon};
  config.attack_sequences = {sequence};
  config.abilities = std::move(abilities);
  config.alignment = alignment;

  return config;
}

inline entities::EntityConfig MakeMultiAttackEntityConfig(
    const std::string& name, int hp, int ac, int fortification,
    items::Weapon weapon, int num_attacks,
    std::vector<abilities::Ability> abilities = {},
    entities::Resistances resistances = {}) {
  combat::AttackMove move;
  move.weapon = weapon;
  move.attack_modifier = 0;
  move.damage_modifier = 0;
  move.crit_range_bonus = 0;
  move.crit_multiplier_bonus = 0;

  combat::AttackSequence sequence;
  sequence.name = "Default";
  for (int i = 0; i < num_attacks; ++i) {
    sequence.attacks.push_back(move);
  }
  sequence.attack_modifier = 0;
  sequence.damage_modifier = 0;

  entities::EntityConfig config;
  config.name = name;
  config.starting_stats.base_stats.hp = hp;
  config.starting_stats.base_stats.armour_class = ac;
  config.starting_stats.base_stats.fortification = fortification;
  config.starting_stats.base_stats.resistances = resistances;
  config.equipped_weapons = {weapon};
  config.attack_sequences = {sequence};
  config.abilities = std::move(abilities);
  config.alignment = rules::Alignment::TrueNeutral;

  return config;
}

struct TestHarness {
  std::shared_ptr<dice_rolls::MockRandomEngine> mock_engine;
  std::shared_ptr<dice_rolls::Roller> roller;
  std::unique_ptr<engine::CombatEngine> engine;

  TestHarness() {
    mock_engine = std::make_shared<dice_rolls::MockRandomEngine>();
    roller = std::make_shared<dice_rolls::Roller>(mock_engine);
    engine = std::make_unique<engine::CombatEngine>(roller);
  }
};

}  // namespace testing
}  // namespace ettes

#endif  // TESTS_INTEGRATION_TEST_HELPERS_H_

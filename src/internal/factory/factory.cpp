// -----------------------------------------------------------------------------
// | @file      factory.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/factory/factory.h"

#include <fstream>
#include <functional>
#include <memory>
#include <unordered_map>

#include "internal/abilities/ability.h"
#include "internal/combat/attack.h"
#include "internal/dice_rolls/term.h"
#include "internal/entities/entity.h"
#include "internal/items/enchantment_library.h"
#include "internal/items/weapon.h"
#include "internal/logging/log_manager.h"
#include "internal/rules/damage_types.h"
#include "internal/rules/resistances.h"

#define GLZ_USE_STD_FORMAT_FLOAT 0
#include <glaze/glaze.hpp>

template <>
struct glz::meta<ettes::combat::Effect> {
  using T = ettes::combat::Effect;
  static constexpr auto value =
      glz::object("name", &T::name, "source", &T::source, "trigger",
                  &T::trigger, "is_active", &T::is_active);
};

template <>
struct glz::meta<ettes::items::DamageSource> {
  using T = ettes::items::DamageSource;
  static constexpr auto value = glz::object("name", &T::name);
};

template <>
struct glz::meta<ettes::items::Enchantment> {
  using T = ettes::items::Enchantment;
  static constexpr auto value = glz::object("name", &T::name);
};

template <>
struct glz::meta<ettes::abilities::Ability> {
  using T = ettes::abilities::Ability;
  static constexpr auto value =
      glz::object("name", &T::name, "stack_count", &T::stack_count, "is_active",
                  &T::is_active);
};

namespace ettes {
namespace factory {

using dice_rolls::Dice;
using dice_rolls::Term;
using entities::EntityConfig;
using items::Enchantment;
using items::Weapon;

namespace {

struct AttackMoveRef {
  std::string weapon_ref;
  int attack_modifier = 0;
  int damage_modifier = 0;
  int crit_range_bonus = 0;
  int crit_multiplier_bonus = 0;
};

struct AttackSequenceRef {
  std::string name;
  std::vector<AttackMoveRef> attacks;
  int attack_modifier = 0;
  int damage_modifier = 0;
};

struct EntityConfigRef {
  std::string name;
  entities::Stats starting_stats;
  std::vector<items::Weapon> equipped_weapons;
  std::vector<AttackSequenceRef> attack_sequences;
  std::vector<abilities::Ability> abilities;
  rules::Alignment alignment;
};

const std::unordered_map<std::string, std::function<Enchantment()>>
    kEnchantmentRegistry = {
        {"FlamingWeapon", items::CreateFlamingEnchantment},
        {"Vampiric", items::CreateVampiricEnchantment},
        {"Profane", items::CreateProfaneEnchantment},
        {"Dissonant", items::CreateDissonantEnchantment},
        {"Flaming Explosion", items::CreateFlamingExplosionEnchantment},
        {"Draining", items::CreateDrainingEnchantment},
};

const std::unordered_map<std::string, std::function<abilities::Ability(int)>>
    kAbilityRegistry = {
        {"Erosion", [](int) { return abilities::CreateErosao(); }},
        {"Rigidez Raivosa",
         [](int stacks) {
           auto a = abilities::CreateRigidezRaivosa();
           a.stack_count = stacks;
           return a;
         }},
        {"Trespassar",
         [](int stacks) {
           auto a = abilities::CreateTrespassar();
           a.stack_count = stacks;
           return a;
         }},
        {"Duro de Ferir",
         [](int stacks) { return abilities::CreateDuroDeFerir(stacks); }},
        {"Duro de Matar",
         [](int stacks) { return abilities::CreateDuroDeMatar(stacks); }},
};

}  // namespace

Factory::Factory(std::string player_filename, Monster monster_type)
    : player_filename_(std::move(player_filename)),
      monster_type_(monster_type),
      player_config_(GetCharacterFromJSON(player_filename_)),
      monster_config_(MonsterFactory(monster_type_)) {
}

std::shared_ptr<entities::IEntity> Factory::CreatePlayer() const {
  return std::make_shared<entities::Entity>(player_config_);
}

std::shared_ptr<entities::IEntity> Factory::CreateMonster() const {
  return std::make_shared<entities::Entity>(monster_config_);
}

EntityConfig Factory::MonsterFactory(Monster monster_type) const {
  switch (monster_type) {
    case Uktril:
      return GetCharacterFromJSON("uktril.json");
    case Geraktril:
      return GetCharacterFromJSON("geraktril.json");
    case Reishid:
      return GetCharacterFromJSON("reishid.json");
    case Custom:
      return GetCharacterFromJSON("custom_monster.json");
  }
  return EntityConfig{};
}

EntityConfig Factory::GetCharacterFromJSON(const std::string& filename) const {
  auto config = LoadCharacterFromJSON(filename);

  for (auto& weapon : config.equipped_weapons) {
    std::vector<Enchantment> rebuilt_enchantments;
    for (const auto& ench : weapon.enchantments) {
      rebuilt_enchantments.push_back(RebuildEnchantmentFromName(ench.name));
    }
    weapon.enchantments = rebuilt_enchantments;
  }

  std::vector<abilities::Ability> rebuilt_abilities;
  for (const auto& ability : config.abilities) {
    rebuilt_abilities.push_back(
        RebuildAbilityFromName(ability.name, ability.stack_count));
  }

  config.abilities = rebuilt_abilities;

  for (auto& attack_sequence : config.attack_sequences) {
    for (auto& attack_move : attack_sequence.attacks) {
      for (const auto& equipped_weapon : config.equipped_weapons) {
        if (attack_move.weapon.name == equipped_weapon.name) {
          attack_move.weapon = equipped_weapon;
        }
      }
    }
  }

  return config;
}

EntityConfig Factory::LoadCharacterFromJSON(const std::string& filename) const {
  auto* logger = logging::LogManager::GetLogger("factory");
  const std::string path = "resources/" + filename;

  std::string file_contents;
  {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) {
      logger->Error("Failed to open file: {}", path);
      return EntityConfig{};
    }
    file_contents.assign(std::istreambuf_iterator<char>(ifs),
                         std::istreambuf_iterator<char>());
  }

  EntityConfigRef parsed{};
  auto ec = glz::read_json(parsed, file_contents);
  if (ec) {
    logger->Error("Failed to parse JSON from {}: {}", path,
                  glz::format_error(ec, file_contents));
    return EntityConfig{};
  }

  EntityConfig config;
  config.name = parsed.name;
  config.starting_stats = parsed.starting_stats;
  config.equipped_weapons = parsed.equipped_weapons;
  config.abilities = parsed.abilities;
  config.alignment = parsed.alignment;

  for (const auto& seq_ref : parsed.attack_sequences) {
    combat::AttackSequence seq;
    seq.name = seq_ref.name;
    seq.attack_modifier = seq_ref.attack_modifier;
    seq.damage_modifier = seq_ref.damage_modifier;
    for (const auto& move_ref : seq_ref.attacks) {
      combat::AttackMove move;
      move.weapon.name = move_ref.weapon_ref;
      move.attack_modifier = move_ref.attack_modifier;
      move.damage_modifier = move_ref.damage_modifier;
      move.crit_range_bonus = move_ref.crit_range_bonus;
      move.crit_multiplier_bonus = move_ref.crit_multiplier_bonus;
      seq.attacks.push_back(move);
    }
    config.attack_sequences.push_back(seq);
  }

  return config;
}

Enchantment Factory::RebuildEnchantmentFromName(const std::string& name) const {
  auto it = kEnchantmentRegistry.find(name);
  if (it != kEnchantmentRegistry.end()) {
    return it->second();
  }
  return Enchantment{};
}

abilities::Ability Factory::RebuildAbilityFromName(const std::string& name,
                                                   int stack_count) const {
  auto it = kAbilityRegistry.find(name);
  if (it != kAbilityRegistry.end()) {
    return it->second(stack_count);
  }
  return abilities::Ability{};
}

EntityConfig Factory::CreateExampleCharacter() const {
  Weapon machado{
      .name = "Machado",
      .attack_bonus = 2,
      .damage = Term{.dice_groups = {Dice{.count = 4, .sides = 8}}, .bonus = 0},
      .damage_type = rules::DamageType::Slash,
      .damage_modifier = rules::DamageModifier::None,
      .crit_range = 20,
      .crit_multiplier = 3,
      .enchantments = {ettes::items::CreateVampiricEnchantment()},
  };

  auto character_config = entities::EntityConfig{
      .name = "Character",
      .starting_stats =
          entities::Stats{
              .base_stats =
                  entities::BaseStats{
                      .hp = 160,
                      .armour_class = 21,
                      .fortification = 0,
                      .attack_bonuses =
                          entities::AttackBonuses{
                              .attack_bonus = 19,
                              .damage_bonus = 21,
                          },
                      .resistances = entities::Resistances{},
                  },
              .bonus_stats =
                  entities::BonusStats{
                      .temporary_hp = 0,
                      .ac_bonus = 0,
                      .bonus_resistances = entities::Resistances{},
                  },
          },
      .equipped_weapons = {machado},
      .attack_sequences =
          std::vector<combat::AttackSequence>{
              combat::AttackSequence{
                  .name = "Normal Attack",
                  .attacks =
                      std::vector<combat::AttackMove>{
                          combat::AttackMove{.weapon = machado,
                                             .attack_modifier = 0,
                                             .damage_modifier = 0},
                      },
                  .attack_modifier = 0,
                  .damage_modifier = 0,
              },
              combat::AttackSequence{
                  .name = "Powerful Attack",
                  .attacks =
                      std::vector<combat::AttackMove>{
                          combat::AttackMove{.weapon = machado,
                                             .attack_modifier = 0,
                                             .damage_modifier = 0},
                      },
                  .attack_modifier = -2,
                  .damage_modifier = 4,
              },
          },
      .abilities =
          std::vector<abilities::Ability>{
              abilities::CreateErosao(),
              abilities::CreateRigidezRaivosa(),
              abilities::CreateTrespassar(),
          },
      .alignment = rules::Alignment::ChaoticNeutral,
  };

  SaveCharacterToJSON(character_config, "resources/example_character.json");

  return character_config;
}

EntityConfig Factory::CreateCustomEnemy() const {
  Weapon machado{
      .name = "Machandejante",
      .attack_bonus = 2,
      .damage = Term{.dice_groups = {Dice{.count = 3, .sides = 6}}, .bonus = 1},
      .damage_type = rules::DamageType::Slash,
      .damage_modifier = rules::DamageModifier::None,
      .crit_range = 20,
      .crit_multiplier = 3,
      .enchantments = {ettes::items::CreateFlamingExplosionEnchantment()},
  };

  auto character_config =
      entities::EntityConfig{
          .name = "Darius Magnus",
          .starting_stats =
              entities::Stats{
                  .base_stats =
                      entities::BaseStats{
                          .hp = 130,
                          .armour_class = 27,
                          .fortification = 0,
                          .attack_bonuses =
                              entities::AttackBonuses{
                                  .attack_bonus = 18,
                                  .damage_bonus = 26,
                              },
                          .resistances =
                              entities::Resistances{
                                  .damage_reductions =
                                      std::vector<rules::DamageReduction>{
                                          rules::DamageReduction{
                                              .bypass_modifiers =
                                                  rules::DamageModifier::None,
                                              .amount = 13,
                                          },
                                      }}},
                  .bonus_stats =
                      entities::BonusStats{
                          .temporary_hp = 0,
                          .ac_bonus = 0,
                          .bonus_resistances = entities::Resistances{},
                      },
              },
          .equipped_weapons = {machado},
          .attack_sequences =
              std::vector<combat::AttackSequence>{
                  combat::AttackSequence{
                      .name = "Normal Attack",
                      .attacks =
                          std::vector<combat::AttackMove>{
                              combat::AttackMove{
                                  .weapon = machado,
                                  .attack_modifier = 0,
                                  .damage_modifier = 0},
                          },
                      .attack_modifier = 0,
                      .damage_modifier = 0,
                  },
                  combat::AttackSequence{
                      .name = "Powerful Attack",
                      .attacks =
                          std::vector<combat::AttackMove>{
                              combat::AttackMove{
                                  .weapon = machado,
                                  .attack_modifier = 0,
                                  .damage_modifier = 0},
                          },
                      .attack_modifier = -2,
                      .damage_modifier = 4,
                  },
              },
          .abilities = std::vector<abilities::Ability>{},
          .alignment = rules::Alignment::LawfulGood,
      };

  SaveCharacterToJSON(character_config, "resources/custom_monster.json");

  return character_config;
}

void Factory::SaveCharacterToJSON(
    const entities::EntityConfig& character_config,
    const std::string& filename) const {
  EntityConfigRef json_config;
  json_config.name = character_config.name;
  json_config.starting_stats = character_config.starting_stats;
  json_config.equipped_weapons = character_config.equipped_weapons;
  json_config.abilities = character_config.abilities;
  json_config.alignment = character_config.alignment;

  for (const auto& seq : character_config.attack_sequences) {
    AttackSequenceRef seq_ref;
    seq_ref.name = seq.name;
    seq_ref.attack_modifier = seq.attack_modifier;
    seq_ref.damage_modifier = seq.damage_modifier;
    for (const auto& move : seq.attacks) {
      AttackMoveRef move_ref;
      move_ref.weapon_ref = move.weapon.name;
      move_ref.attack_modifier = move.attack_modifier;
      move_ref.damage_modifier = move.damage_modifier;
      move_ref.crit_range_bonus = move.crit_range_bonus;
      move_ref.crit_multiplier_bonus = move.crit_multiplier_bonus;
      seq_ref.attacks.push_back(move_ref);
    }
    json_config.attack_sequences.push_back(seq_ref);
  }

  std::string buffer;
  auto error = glz::write_file_json(json_config, filename, buffer);
  if (error.ec != glz::error_code::none) {
    logging::LogManager::GetLogger("factory")->Error(
        "Failed to write JSON to {}", filename);
  }
}

}  // namespace factory
}  // namespace ettes

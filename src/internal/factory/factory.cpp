// -----------------------------------------------------------------------------
// | @file      factory.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/factory/factory.h"

#include <memory>
#include "internal/rules/resistances.h"

#define GLZ_USE_STD_FORMAT_FLOAT 0
#include <glaze/glaze.hpp>

#include "internal/abilities/ability.h"
#include "internal/combat/attack.h"
#include "internal/dice_rolls/roller.h"
#include "internal/entities/entity.h"
#include "internal/items/enchantment_library.h"
#include "internal/items/weapon.h"
#include "internal/rules/damage_types.h"

namespace internal {
namespace factory {

using dice_rolls::Dice;
using dice_rolls::Term;
using entities::Entity;
using entities::EntityConfig;
using items::Enchantment;
using items::Weapon;

std::shared_ptr<Entity> MonsterFactory(Monster monsterType) {
  switch (monsterType) {
    case Uktril: {
      std::shared_ptr<Weapon> pinca = std::make_shared<Weapon>(Weapon{
          .name = "pinca",
          .attack_bonus = 1,
          .damage =
              Term{
                  .dice_groups = {Dice{.count = 1, .sides = 8}},
              },
          .damage_type = rules::DamageType::Slash,
          .crit_range = 20,
          .crit_multiplier = 2,
      });

      std::shared_ptr<Weapon> garra = std::make_shared<Weapon>(
          Weapon{.name = "garra",
                 .attack_bonus = 0,
                 .damage =
                     Term{
                         .dice_groups = {Dice{.count = 1, .sides = 4}},
                     },
                 .damage_type = rules::DamageType::Slash,
                 .crit_range = 20,
                 .crit_multiplier = 2});

      auto uktril_config = entities::EntityConfig{
          .name = "Uktril",
          .starting_stats =
              entities::Stats{
                  .base_stats =
                      entities::BaseStats{
                          .hp = 60,
                          .armour_class = 22,
                          .fortification = 100,

                          .attack_bonuses =
                              entities::AttackBonuses{
                                  .attack_bonus = 12,
                                  .damage_bonus = 8,
                              },
                          .resistances =
                              entities::Resistances{
                                  .damage_reductions = {rules::DamageReduction{
                                      .bypass_modifiers =
                                          rules::DamageModifier::Magic,
                                      .amount = 5,
                                  }},
                                  .immunity =
                                      rules::Immunity{
                                          .immune_categories =
                                              rules::DamageCategory::Energy,
                                      },
                              },
                      },
              },
          .equipped_weapons = {pinca, garra},
          .attack_sequences =
              std::vector<combat::AttackSequence>{
                  combat::AttackSequence{
                      .name = "Multiple Attacks",
                      .attacks =
                          std::vector<combat::AttackMove>{
                              combat::AttackMove{
                                  .weapon = pinca,
                              },
                              combat::AttackMove{
                                  .weapon = garra,
                              },
                          },
                      .attack_modifier = 0,
                      .damage_modifier = 0,
                  },
              },
          .abilities = {},
          .alignment = rules::Alignment::ChaoticEvil,
      };

      return std::make_shared<Entity>(uktril_config);
    }
    case Geraktril: {
      std::shared_ptr<Weapon> pinca = std::make_shared<Weapon>(
          Weapon{.name = "pinca",
                 .attack_bonus = 1,
                 .damage = Term{.dice_groups = {Dice{.count = 1, .sides = 8}}},
                 .damage_type = rules::DamageType::Slash,
                 .crit_range = 20,
                 .crit_multiplier = 2});

      std::shared_ptr<Weapon> garra = std::make_shared<Weapon>(
          Weapon{.name = "garra",
                 .attack_bonus = 0,
                 .damage = Term{.dice_groups = {Dice{.count = 1, .sides = 4}}},
                 .damage_type = rules::DamageType::Slash,
                 .crit_range = 20,
                 .crit_multiplier = 2});

      auto geraktril_config = entities::EntityConfig{
          .name = "Geraktril",
          .starting_stats =
              entities::Stats{
                  .base_stats =
                      entities::BaseStats{
                          .hp = 99,
                          .armour_class = 25,
                          .fortification = 100,

                          .attack_bonuses =
                              entities::AttackBonuses{
                                  .attack_bonus = 16,
                                  .damage_bonus = 10,
                              },
                          .resistances =
                              entities::Resistances{
                                  .damage_reductions = {rules::DamageReduction{
                                      .bypass_modifiers =
                                          rules::DamageModifier::Magic,
                                      .amount = 10,
                                  }},
                                  .immunity =
                                      rules::Immunity{
                                          .immune_categories =
                                              rules::DamageCategory::Energy,
                                      },
                              },
                      },
              },
          .equipped_weapons = {pinca, garra},
          .attack_sequences =
              std::vector<combat::AttackSequence>{
                  combat::AttackSequence{
                      .name = "Multiple Attacks",
                      .attacks =
                          std::vector<combat::AttackMove>{
                              combat::AttackMove{.weapon = pinca},
                              combat::AttackMove{.weapon = garra},
                          },
                      .attack_modifier = 0,
                      .damage_modifier = 0,
                  }},
          .abilities = {},
          .alignment = rules::Alignment::ChaoticEvil,
      };

      return std::make_shared<Entity>(geraktril_config);
    }
    case Reishid: {
      std::shared_ptr<Weapon> adaga = std::make_shared<Weapon>(Weapon{
          .name = "adaga",
          .attack_bonus = 4,
          .damage =
              Term{.dice_groups = {Dice{.count = 1, .sides = 4}}, .bonus = 4},
          .damage_type = rules::DamageType::Pierce,
          .damage_modifier = rules::DamageModifier::Magic,
          .crit_range = 19,
          .crit_multiplier = 2,
          .enchantments = {internal::items::CreateProfaneEnchantment()}});

      std::shared_ptr<Weapon> mordida = std::make_shared<Weapon>(Weapon{
          .name = "mordida",
          .attack_bonus = 0,
          .damage =
              Term{.dice_groups = {Dice{.count = 1, .sides = 4}}, .bonus = 4},
          .damage_type = rules::DamageType::Pierce,
          .crit_range = 20,
          .crit_multiplier = 2});

      std::shared_ptr<Weapon> garra = std::make_shared<Weapon>(Weapon{
          .name = "garra",
          .attack_bonus = 0,
          .damage =
              Term{.dice_groups = {Dice{.count = 1, .sides = 4}}, .bonus = 0},
          .damage_type = rules::DamageType::Slash,
          .crit_range = 20,
          .crit_multiplier = 2});

      auto reishid_config = entities::EntityConfig{
          .name = "Reishid",
          .starting_stats =
              entities::Stats{
                  .base_stats =
                      entities::BaseStats{
                          .hp = 143,
                          .armour_class = 30,
                          .fortification = 100,

                          .attack_bonuses =
                              entities::AttackBonuses{
                                  .attack_bonus = 22,
                                  .damage_bonus = 10,
                              },
                          .resistances =
                              entities::Resistances{
                                  .damage_reductions = {rules::DamageReduction{
                                      .bypass_modifiers =
                                          rules::DamageModifier::Magic,
                                      .amount = 10,
                                  }},
                                  .immunity =
                                      rules::Immunity{
                                          .immune_categories =
                                              rules::DamageCategory::Energy,
                                      },
                              },
                      },
              },
          .equipped_weapons = {adaga, mordida, garra},
          .attack_sequences =
              std::vector<combat::AttackSequence>{
                  combat::AttackSequence{
                      .name = "Multiple Attacks",
                      .attacks =
                          std::vector<combat::AttackMove>{
                              combat::AttackMove{.weapon = adaga},
                              combat::AttackMove{.weapon = mordida},
                              combat::AttackMove{.weapon = garra},
                          },
                      .attack_modifier = 0,
                      .damage_modifier = 0,
                  },
              },
          .abilities = {},
          .alignment = rules::Alignment::ChaoticEvil,
      };

      return std::make_shared<Entity>(reishid_config);
    }
    case Custom:
      return GetCharacterFromJSON("custom_monster.json");
    default:
      return MonsterFactory(Monster::Uktril);
  }
}

std::shared_ptr<Entity> GetCharacterFromJSON(const std::string& filename) {
  auto config = LoadCharacterFromJSON(filename);

  for (auto& weapon : config.equipped_weapons) {
    std::vector<Enchantment> rebuilt_enchantments;
    for (const auto& ench : weapon->enchantments) {
      rebuilt_enchantments.push_back(RebuildEnchantmentFromName(ench.name));
    }
    weapon->enchantments = rebuilt_enchantments;
  }

  std::vector<abilities::Ability> rebuilt_abilities;
  for (const auto& ability : config.abilities) {
    rebuilt_abilities.push_back(
        RebuildAbilityFromName(ability.name, ability.stack_count));
  }

  config.abilities = rebuilt_abilities;

  for (auto& attack_sequence : config.attack_sequences) {
    for (auto& attack_move : attack_sequence.attacks) {
      if (attack_move.weapon) {
        for (const auto& equipped_weapon : config.equipped_weapons) {
          if (attack_move.weapon->name == equipped_weapon->name) {
            attack_move.weapon = equipped_weapon;
          }
        }
      }
    }
  }

  return std::make_shared<Entity>(config);
}

EntityConfig LoadCharacterFromJSON(const std::string& filename) {
  std::string file_contents;
  {
    std::ifstream ifs("resources/" + filename, std::ios::binary);
    file_contents.assign(std::istreambuf_iterator<char>(ifs),
                         std::istreambuf_iterator<char>());
  }

  EntityConfig parsed{};
  auto ec = glz::read_json(parsed, file_contents);
  if (ec) {
    return EntityConfig{};
  }

  return parsed;
}

Enchantment RebuildEnchantmentFromName(const std::string& name) {
  if (name == "FlamingWeapon") {
    return items::CreateFlamingEnchantment();
  } else if (name == "Vampiric") {
    return items::CreateVampiricEnchantment();
  } else if (name == "Profane") {
    return items::CreateProfaneEnchantment();
  } else if (name == "Dissonant") {
    return items::CreateDissonantEnchantment();
  } else if (name == "Flaming Explosion") {
    return items::CreateFlamingExplosionEnchantment();
  } else if (name == "Draining") {
    return items::CreateDrainingEnchantment();
  }
  // Unknown enchantment, return empty
  return Enchantment{};
}

abilities::Ability RebuildAbilityFromName(const std::string& name,
                                          int stack_count) {
  if (name == "Erosion") {
    return abilities::CreateErosao();
  } else if (name == "Rigidez Raivosa") {
    auto ability = abilities::CreateRigidezRaivosa();
    ability.stack_count = stack_count;
    return ability;
  } else if (name == "Trespassar") {
    auto ability = abilities::CreateTrespassar();
    ability.stack_count = stack_count;
    return ability;
  } else if (name == "Duro de Ferir") {
    return abilities::CreateDuroDeFerir(stack_count);
  } else if (name == "Duro de Matar") {
    return abilities::CreateDuroDeMatar(stack_count);
  }
  // Unknown ability, return empty
  return abilities::Ability{};
}

std::shared_ptr<Entity> CreateExampleCharacter() {
  std::shared_ptr<Weapon> machado = std::make_shared<Weapon>(Weapon{
      .name = "Machado",
      .attack_bonus = 2,
      .damage = Term{.dice_groups = {Dice{.count = 4, .sides = 8}}, .bonus = 0},
      .damage_type = rules::DamageType::Slash,
      .damage_modifier = rules::DamageModifier::None,
      .crit_range = 20,
      .crit_multiplier = 3,
      .enchantments = {internal::items::CreateVampiricEnchantment()},
  });

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

  return std::make_shared<Entity>(character_config);
}

std::shared_ptr<Entity> CreateCustomEnemy() {
  std::shared_ptr<Weapon> machado = std::make_shared<Weapon>(Weapon{
      .name = "Machandejante",
      .attack_bonus = 2,
      .damage = Term{.dice_groups = {Dice{.count = 3, .sides = 6}}, .bonus = 1},
      .damage_type = rules::DamageType::Slash,
      .damage_modifier = rules::DamageModifier::None,
      .crit_range = 20,
      .crit_multiplier = 3,
      .enchantments = {internal::items::CreateFlamingExplosionEnchantment()},
  });

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

  return std::make_shared<Entity>(character_config);
}

void SaveCharacterToJSON(const entities::EntityConfig& character_config,
                         const std::string& filename) {
  std::string buffer;
  auto error = glz::write_file_json(character_config, filename, buffer);
  if (error.ec != glz::error_code::none) {
    return;
  }
}

}  // namespace factory
}  // namespace internal

// -----------------------------------------------------------------------------
// | @file      factory.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/factory/factory.h"

#include <memory>

#define GLZ_USE_STD_FORMAT_FLOAT 0

#include <iostream>
#include <glaze/glaze.hpp>

#include "internal/combat/attack.h"
#include "internal/dice_rolls/roller.h"
#include "internal/entities/entity.h"
#include "internal/items/weapon.h"
#include "internal/rules/damage_types.h"

namespace internal {
namespace factory {

using dice_rolls::Dice;
using dice_rolls::Term;
using items::Weapon;

Entity MonsterFactory(Monster monsterType) {
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

      std::shared_ptr<Weapon> garra = std::make_shared<Weapon>(Weapon{
          .name = "garra",
          .attack_bonus = 0,
          .damage =
              Term{
                  .dice_groups = {Dice{.count = 1, .sides = 4}},
              },
          .damage_type = rules::DamageType::Slash,
          .crit_range = 20,
          .crit_multiplier = 2,
      });

      auto uktril_config =
          entities::EntityConfig{
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

                              .damage_reductions = {rules::DamageReduction{
                                  .bypass_modifiers =
                                      {rules::DamageModifier::Magic},
                                  .amount = 5,
                              }},
                              .immunity =
                                  rules::Immunity{
                                      .immune_categories =
                                          std::vector<rules::DamageCategory>{
                                              rules::DamageCategory::Energy,
                                          },
                                  },
                          },
                  },

              .equipped_weapons = {pinca, garra},
              .attack_sequences =
                  std::vector<combat::AttackSequence>{
                      combat::AttackSequence{
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

              .alignment = rules::Alignment::ChaoticEvil,
          };

      return entities::Entity{uktril_config};
    }
    case Geraktril: {
      std::shared_ptr<Weapon> pinca = std::make_shared<Weapon>(Weapon{
          .name = "pinca",
          .attack_bonus = 1,
          .damage = Term{.dice_groups = {Dice{.count = 1, .sides = 8}}},
          .damage_type = rules::DamageType::Slash,
          .crit_range = 20,
          .crit_multiplier = 2,
      });

      std::shared_ptr<Weapon> garra = std::make_shared<Weapon>(Weapon{
          .name = "garra",
          .attack_bonus = 0,
          .damage = Term{.dice_groups = {Dice{.count = 1, .sides = 4}}},
          .damage_type = rules::DamageType::Slash,
          .crit_range = 20,
          .crit_multiplier = 2,
      });

      auto geraktril_config =
          entities::EntityConfig{
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

                              .damage_reductions = {rules::DamageReduction{
                                  .bypass_modifiers =
                                      {rules::DamageModifier::Magic},
                                  .amount = 10,
                              }},
                              .immunity =
                                  rules::Immunity{
                                      .immune_categories =
                                          std::vector<rules::DamageCategory>{
                                              rules::DamageCategory::Energy,
                                          },
                                  },
                          },
                  },
              .equipped_weapons = {pinca, garra},
              .attack_sequences =
                  std::vector<combat::AttackSequence>{
                      combat::AttackSequence{
                          .attacks =
                              std::vector<combat::AttackMove>{
                                  combat::AttackMove{.weapon = pinca},
                                  combat::AttackMove{.weapon = garra},
                              },
                          .attack_modifier = 0,
                          .damage_modifier = 0,
                      }},
              .alignment = rules::Alignment::ChaoticEvil,
          };

      return entities::Entity{geraktril_config};
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
      });

      std::shared_ptr<Weapon> mordida = std::make_shared<Weapon>(Weapon{
          .name = "mordida",
          .attack_bonus = 0,
          .damage =
              Term{.dice_groups = {Dice{.count = 1, .sides = 4}}, .bonus = 4},
          .damage_type = rules::DamageType::Pierce,
          .crit_range = 20,
          .crit_multiplier = 2,
      });

      std::shared_ptr<Weapon> garra = std::make_shared<Weapon>(Weapon{
          .name = "garra",
          .attack_bonus = 0,
          .damage =
              Term{.dice_groups = {Dice{.count = 1, .sides = 4}}, .bonus = 0},
          .damage_type = rules::DamageType::Slash,
          .crit_range = 20,
          .crit_multiplier = 2,
      });

      auto reishid_config =
          entities::EntityConfig{
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

                              .damage_reductions = {rules::DamageReduction{
                                  .bypass_modifiers =
                                      {rules::DamageModifier::Magic},
                                  .amount = 10,
                              }},
                              .immunity =
                                  rules::Immunity{
                                      .immune_categories =
                                          std::vector<rules::DamageCategory>{
                                              rules::DamageCategory::Energy,
                                          },
                                  },
                          },
                  },
              .equipped_weapons = {adaga, mordida, garra},
              .attack_sequences =
                  std::vector<combat::AttackSequence>{
                      combat::AttackSequence{
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
              .alignment = rules::Alignment::ChaoticEvil,
          };

      return entities::Entity{reishid_config};
    }
    case Custom:
      return entities::Entity{LoadCharacterFromJSON("resources/custom_monster.json")};
    default:
      return MonsterFactory(Monster::Uktril);
  }
}

Entity GetPlayer(const std::string& filename) {
    auto config = LoadCharacterFromJSON(filename);
    return entities::Entity{config};
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
    std::cerr << "Failed to parse JSON: "
              << glz::format_error(ec, file_contents) << std::endl;
    return EntityConfig{};
  }

  return parsed;
}

}  // namespace factory
}  // namespace internal

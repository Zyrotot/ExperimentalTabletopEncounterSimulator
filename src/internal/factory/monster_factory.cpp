// -----------------------------------------------------------------------------
// | @file      monster_factory.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/factory/monster_factory.h"
#include <memory>

#include "internal/combat/attack.h"
#include "internal/entities/entity.h"
#include "internal/rules/damage_types.h"
#include "internal/items/weapon.h"
#include "internal/dice_rolls/roller.h"

namespace internal {
namespace factory {

using items::Weapon;
using dice_rolls::Term;
using dice_rolls::Dice;

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
              .attack_moves =
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
      auto geraktril_config = entities::EntityConfig{
          .name = "Geraktril",
          .starting_stats =
              entities::Stats{
                  .base_stats =
                      entities::BaseStats{
                          .hp = 99,
                          .armour_class = 25,
                          .fortification = 100,

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
          .alignment = rules::Alignment::ChaoticEvil,
      };

      return entities::Entity{geraktril_config};
    }
    case Reishid: {
      auto reishid_config = entities::EntityConfig{
          .name = "Reishid",
          .starting_stats =
              entities::Stats{
                  .base_stats =
                      entities::BaseStats{
                          .hp = 143,
                          .armour_class = 30,
                          .fortification = 100,

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
          .alignment = rules::Alignment::ChaoticEvil,
      };

      return entities::Entity{reishid_config};
    }
    default:
      // custom_enemy, err := LoadCharacterFromJSON("custom_enemy.json")
      // if err == nil {
      // return custom_enemy
      // }
      return MonsterFactory(Monster::Uktril);
  }
}

// func LoadCharacterFromJSON(filename string) (*entity.Character, error) {
//    data, err := os.ReadFile(filename)
//    if err != nil {
//       return nil, err
//    }

//    var c entity.Character
//    if err := json.Unmarshal(data, &c); err != nil {
//       return nil, err
//    }

//    // Initialize runtime values
//    c.CurrentHP = c.MaxHP
//    c.Immortal = 0
//    c.IsNPC = false
//    c.TemporaryBonus = TempBonus{0, 0, 0}

//    return &c, nil
// }

}  // namespace factory
}  // namespace internal

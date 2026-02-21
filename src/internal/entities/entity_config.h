// -----------------------------------------------------------------------------
// | @file      entity_config.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_ENTITY_CONFIG_H_
#define SRC_INTERNAL_ENTITIES_ENTITY_CONFIG_H_

#include <string>
#include <vector>

#include "internal/abilities/ability.h"
#include "internal/combat/attack.h"
#include "internal/entities/stats.h"
#include "internal/items/weapon.h"
#include "internal/rules/alignment.h"

namespace internal {
namespace entities {

struct EntityConfig {
  std::string name;

  Stats starting_stats;

  std::vector<items::Weapon> equipped_weapons;
  std::vector<combat::AttackSequence> attack_sequences;
  std::vector<abilities::Ability> abilities;

  rules::Alignment alignment;
};

}  // namespace entities
}  // namespace internal

#endif  // SRC_INTERNAL_ENTITIES_ENTITY_CONFIG_H_

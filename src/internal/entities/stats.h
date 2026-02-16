// -----------------------------------------------------------------------------
// | @file      stats.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_STATS_H_
#define SRC_INTERNAL_ENTITIES_STATS_H_

#include <vector>

#include "internal/rules/resistances.h"

namespace internal {
namespace entities {

using rules::DamageReduction;
using rules::EnergyResistance;
using rules::Immunity;
using rules::Weakness;

struct AttackBonuses {
  int attack_bonus = 0;
  int damage_bonus = 0;
};

struct BaseStats {
  int hp = 0;
  int armour_class = 0;
  int fortification = 0;

  AttackBonuses attack_bonuses = {};

  std::vector<DamageReduction> damage_reductions = {};
  std::vector<EnergyResistance> energy_resistances = {};
  Immunity immunity = {};
  std::vector<Weakness> weaknesses = {};
};

struct BonusStats {
  int temporary_hp = 0;
  int ac_bonus = 0;
};

struct Stats {
  BaseStats base_stats = {};
  BonusStats bonus_stats = {};
};

}  // namespace entities
}  // namespace internal

#endif  // SRC_INTERNAL_ENTITIES_STATS_H_

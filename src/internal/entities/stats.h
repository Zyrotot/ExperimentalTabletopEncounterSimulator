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
  int attack_bonus;
  int damage_bonus;
};

struct BaseStats {
  int hp;
  int armour_class;
  int fortification;

  AttackBonuses attack_bonuses;

  std::vector<DamageReduction> damage_reductions;
  std::vector<EnergyResistance> energy_resistances;
  Immunity immunity;
  std::vector<Weakness> weaknesses;
};

struct BonusStats {
  int temporary_hp;
  int ac_bonus;
};

struct Stats {
  BaseStats base_stats;
  BonusStats bonus_stats;
};

}  // namespace entities
}  // namespace internal

#endif  // SRC_INTERNAL_ENTITIES_STATS_H_

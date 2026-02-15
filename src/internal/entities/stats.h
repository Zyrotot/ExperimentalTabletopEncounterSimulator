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

struct BaseStats {
  int hp_;
  int armour_class_;
  int fortification_;

  std::vector<DamageReduction> damage_reductions_;
  std::vector<EnergyResistance> energy_resistances_;
  std::vector<Immunity> immunities_;
  std::vector<Weakness> weaknesses_;
};

struct BonusStats {
  int temporary_hp_;
  int ac_bonus_;
};

struct Stats {
  BaseStats base_stats_;
  BonusStats bonus_stats_;
};

}  // namespace entities
}  // namespace internal

#endif  // SRC_INTERNAL_ENTITIES_STATS_H_

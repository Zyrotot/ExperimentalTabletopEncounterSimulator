// -----------------------------------------------------------------------------
// | @file      resistances.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RULES_RESISTANCES_H_
#define SRC_INTERNAL_RULES_RESISTANCES_H_

#include <vector>

#include "internal/rules/damage_types.h"

namespace internal {
namespace rules {

struct DamageReduction {
  std::vector<DamageType> bypass_types;
  std::vector<DamageModifier> bypass_modifiers;
  int amount;
};

struct EnergyResistance {
  DamageType resistance_type;
  int amount;
};

struct Immunity {
  std::vector<DamageType> immune_types;
  std::vector<DamageCategory> immune_categories;
};

struct Weakness {
  std::vector<DamageType> weak_types;
  int ammount;
};

}  // namespace rules
}  // namespace internal

#endif  // SRC_INTERNAL_RULES_RESISTANCES_H_

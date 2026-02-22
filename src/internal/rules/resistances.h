// -----------------------------------------------------------------------------
// | @file      resistances.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RULES_RESISTANCES_H_
#define SRC_INTERNAL_RULES_RESISTANCES_H_

#include "internal/rules/damage_types.h"

namespace ettes {
namespace rules {

struct DamageReduction {
  DamageType bypass_types = DamageType::None;
  DamageModifier bypass_modifiers = DamageModifier::None;
  int amount = 0;
};

struct EnergyResistance {
  DamageType resistance_type;
  int amount = 0;
};

struct Immunity {
  DamageType immune_types = DamageType::None;
  DamageCategory immune_categories = DamageCategory::None;
};

struct Weakness {
  DamageType weak_types = DamageType::None;
  int amount = 0;
};

}  // namespace rules
}  // namespace ettes

#endif  // SRC_INTERNAL_RULES_RESISTANCES_H_

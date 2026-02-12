// -----------------------------------------------------------------------------
// | @file      damage_types.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// ------------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RULES_DAMAGE_TYPES_H_
#define SRC_INTERNAL_RULES_DAMAGE_TYPES_H_

#include <cstdint>

namespace internal {
namespace rules {

enum class DamageType : uint16_t {
  None = 0,

  // Physical damage
  Slash = 1 << 0,
  Pierce = 1 << 1,
  Blunt = 1 << 2,

  // Energy damage
  Fire = 1 << 3,
  Cold = 1 << 4,
  Acid = 1 << 5,
  Electric = 1 << 6,
  Positive = 1 << 7,
  Negative = 1 << 8,
};

enum class DamageTag : uint16_t {
  // Enchantment
  Magic = 1 << 1,

  // Alignment
  Evil = 1 << 2,
  Good = 1 << 3,
  Chaotic = 1 << 4,
  Lawful = 1 << 5,

  // Material
  Silver = 1 << 6,
  Adamantium = 1 << 7,
  RubySteel = 1 << 8,
};

enum class DamageCategory {
  None,
  Physical,
  Energy,
};

inline DamageCategory getDamageCategory(DamageType type) {
  switch (type) {
  case DamageType::Slash:
  case DamageType::Pierce:
  case DamageType::Blunt:
    return DamageCategory::Physical;
  case DamageType::Fire:
  case DamageType::Cold:
  case DamageType::Acid:
  case DamageType::Electric:
    return DamageCategory::Energy;
  case DamageType::None:
  default:
    return DamageCategory::None;
  }
}

}  // namespace rules
}  // namespace internal

#endif  // SRC_INTERNAL_RULES_DAMAGE_TYPES_H_

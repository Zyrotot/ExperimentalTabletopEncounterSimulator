// -----------------------------------------------------------------------------
// | @file      damage_types.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

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

enum class DamageModifier : uint16_t {
  None = 0,

  // Enchantment
  Magic = 1 << 0,

  // Alignment
  Evil = 1 << 1,
  Good = 1 << 2,
  Chaotic = 1 << 3,
  Lawful = 1 << 4,

  // Material
  Silver = 1 << 5,
  Adamantium = 1 << 6,
  RubySteel = 1 << 7,
};

enum class DamageCategory {
  None,
  Physical,
  Energy,
};

struct DamageInstance {
  int amount = 0;
  uint16_t types = 0;
  uint16_t modifiers = 0;
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

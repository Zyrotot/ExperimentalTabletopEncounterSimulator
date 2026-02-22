// -----------------------------------------------------------------------------
// | @file      damage_types.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RULES_DAMAGE_TYPES_H_
#define SRC_INTERNAL_RULES_DAMAGE_TYPES_H_

#include <cstdint>

namespace ettes {
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

constexpr uint16_t kPhysicalMask = static_cast<uint16_t>(DamageType::Slash) |
                                   static_cast<uint16_t>(DamageType::Pierce) |
                                   static_cast<uint16_t>(DamageType::Blunt);

constexpr uint16_t kEnergyMask = static_cast<uint16_t>(DamageType::Fire) |
                                 static_cast<uint16_t>(DamageType::Cold) |
                                 static_cast<uint16_t>(DamageType::Acid) |
                                 static_cast<uint16_t>(DamageType::Electric) |
                                 static_cast<uint16_t>(DamageType::Positive) |
                                 static_cast<uint16_t>(DamageType::Negative);

enum class DamageCategory : uint16_t {
  None = 0,
  Physical = 1 << 0,
  Energy = 1 << 1,
};

struct DamageInstance {
  int amount = 0;
  uint16_t types = 0;
  uint16_t modifiers = 0;
};

inline DamageCategory GetDamageCategory(DamageType type) {
  uint16_t raw = static_cast<uint16_t>(type);

  if (raw & kPhysicalMask) {
    return DamageCategory::Physical;
  }
  if (raw & kEnergyMask) {
    return DamageCategory::Energy;
  }
  return DamageCategory::None;
}

}  // namespace rules
}  // namespace ettes

#endif  // SRC_INTERNAL_RULES_DAMAGE_TYPES_H_

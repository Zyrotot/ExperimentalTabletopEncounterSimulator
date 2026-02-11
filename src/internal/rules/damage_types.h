// -----------------------------------------------------------------------------
// | @file      damage_types.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// ------------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RULES_DAMAGE_TYPES_H
#define SRC_INTERNAL_RULES_DAMAGE_TYPES_H

namespace internal {
namespace rules {

enum class DamageType {
  // Physical damage types
  Slash,
  Pierce,
  Blunt,

  // Elemental damage types
  Fire,
  Cold,
  Acid,
  Electric,

  // Modifier damage types
  Magic,
  Silver,
  Evil,
  Good,
  Chaotic,
  Lawful,
};

enum class DamageCategory {
  Physical,
  Elemental,
  Modifier,
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
    return DamageCategory::Elemental;
  case DamageType::Magic:
  case DamageType::Silver:
  case DamageType::Evil:
  case DamageType::Good:
  case DamageType::Chaotic:
  case DamageType::Lawful:
    return DamageCategory::Modifier;
  }
}

} // namespace rules
} // namespace internal

#endif // SRC_INTERNAL_RULES_DAMAGE_TYPES_H

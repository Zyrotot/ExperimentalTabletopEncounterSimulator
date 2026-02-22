// -----------------------------------------------------------------------------
// | @file      enchantment.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ITEMS_ENCHANTMENT_H_
#define SRC_INTERNAL_ITEMS_ENCHANTMENT_H_

#include <functional>
#include <string>
#include <vector>

#include "internal/combat/effect.h"
#include "internal/rules/damage_types.h"

namespace ettes {

namespace dice_rolls {
class Roller;
}

namespace combat {
struct CombatEventContext;
}

namespace items {

struct DamageSource {
  std::string name;
  std::function<rules::DamageInstance(const combat::CombatEventContext&)>
      contribute;
};

struct Enchantment {
  std::string name;
  std::vector<DamageSource> damage_sources;
  std::vector<combat::Effect> effects;
};

}  // namespace items
}  // namespace ettes

#endif  // SRC_INTERNAL_ITEMS_ENCHANTMENT_H_

// -----------------------------------------------------------------------------
// | @file      effect.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_EFFECT_H_
#define SRC_INTERNAL_COMBAT_EFFECT_H_

#include <functional>
#include <string>

#include "internal/combat/combat_event_types.h"
#include "internal/combat/damage_modification.h"

namespace internal {
namespace combat {

struct CombatEventContext;

struct Effect {
  std::string name;
  std::string source;
  CombatEvent trigger;
  std::function<void(const CombatEventContext&)> on_event;
  std::function<DamageModification(const CombatEventContext&)> on_damage;
  bool is_active = true;
};

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_EFFECT_H_

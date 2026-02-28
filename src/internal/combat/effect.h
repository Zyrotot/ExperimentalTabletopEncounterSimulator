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

namespace ettes {
namespace combat {

struct CombatEventContext;

struct Effect {
  std::string name;
  std::string source;
  CombatEvent trigger;
  std::function<void(const CombatEventContext&)> on_event;
  bool is_active = true;
};

}  // namespace combat
}  // namespace ettes

#endif  // SRC_INTERNAL_COMBAT_EFFECT_H_

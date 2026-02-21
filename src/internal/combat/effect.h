// -----------------------------------------------------------------------------
// | @file      effect.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_EFFECT_H_
#define SRC_INTERNAL_COMBAT_EFFECT_H_

#include <functional>
#include <memory>
#include <string>

#include "internal/combat/combat_event_types.h"

namespace internal {
namespace combat {

class CombatEventContext;

struct Effect {
  std::string name;
  std::string source;
  CombatEvent trigger;
  std::function<void(std::shared_ptr<CombatEventContext>)> on_event;
  bool is_active = true;
};

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_EFFECT_H_

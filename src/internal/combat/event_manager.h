// -----------------------------------------------------------------------------
// | @file      event_manager.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_EVENT_MANAGER_H_
#define SRC_INTERNAL_COMBAT_EVENT_MANAGER_H_

#include <memory>

#include "internal/combat/combat_events.h"

namespace internal {
namespace combat {

class EventManager {
 public:
  static void Emit(CombatEvent event, std::shared_ptr<CombatContext> context);
};

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_EVENT_MANAGER_H_

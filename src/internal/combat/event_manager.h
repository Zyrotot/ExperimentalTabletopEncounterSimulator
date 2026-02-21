// -----------------------------------------------------------------------------
// | @file      event_manager.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_EVENT_MANAGER_H_
#define SRC_INTERNAL_COMBAT_EVENT_MANAGER_H_

#include <vector>

#include "internal/combat/combat_event_types.h"
#include "internal/combat/damage_modification.h"

namespace internal {
namespace combat {

class CombatEventContext;

std::vector<DamageModification> EmitCombatEvent(
    CombatEvent event, CombatEventContext* context);

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_EVENT_MANAGER_H_

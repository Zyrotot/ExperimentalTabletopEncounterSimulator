// -----------------------------------------------------------------------------
// | @file      event_manager.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/combat/event_manager.h"

#include "internal/combat/combat_context.h"
#include "internal/entities/i_entity.h"
#include "internal/logging/log_manager.h"

namespace ettes {
namespace combat {

void EmitCombatEvent(
    CombatEvent event, CombatEventContext* context) {
  if (!context) {
    return;
  }

  static logging::Logger* logger =
      logging::LogManager::GetLogger("events");

  entities::IEntity* relevant_entity = nullptr;

  switch (event) {
    case CombatEvent::TakeDamage:
    case CombatEvent::Heal:
      relevant_entity = context->target;
      break;
    case CombatEvent::DealDamage:
    case CombatEvent::Kill:
    case CombatEvent::Hit:
    case CombatEvent::TurnStart:
      relevant_entity = context->source;
      break;
    default:
      break;
  }

  if (relevant_entity) {
    for (const auto* effect : relevant_entity->GetEffectsForEvent(event)) {
      if (!effect->is_active) {
        continue;
      }

      if (effect->on_event) {
        logger->Debug("Triggering {} effect from {} for {}", effect->name,
                      effect->source, relevant_entity->GetName());
        effect->on_event(*context);
      }
    }
  }

  for (const auto* effect : context->transient_effects) {
    if (!effect->is_active) {
      continue;
    }

    if (effect->trigger != event) {
      continue;
    }

    if (effect->on_event) {
      effect->on_event(*context);
    }
  }
}

}  // namespace combat
}  // namespace ettes

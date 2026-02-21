// -----------------------------------------------------------------------------
// | @file      event_manager.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/combat/event_manager.h"

#include "internal/combat/combat_context.h"
#include "internal/entities/i_entity.h"
#include "internal/logging/log_manager.h"
#include "internal/logging/logger.h"

namespace internal {
namespace combat {

void EventManager::Emit(CombatEvent event,
                        std::shared_ptr<CombatEventContext> context) {
  if (!context) {
    return;
  }

  auto logger = logging::LogManager::GetLogger("events");

  std::vector<std::shared_ptr<entities::IEntity>> relevant_entities;

  switch (event) {
    case CombatEvent::TakeDamage:
      if (context->target)
        relevant_entities.push_back(context->target);
      break;
    case CombatEvent::DealDamage:
    case CombatEvent::Kill:
    case CombatEvent::Hit:
    case CombatEvent::TurnStart:
      if (context->source)
        relevant_entities.push_back(context->source);
      break;
    case CombatEvent::Heal:
      if (context->target)
        relevant_entities.push_back(context->target);
      break;
    default:
      break;
  }

  for (const auto& entity : relevant_entities) {
    if (!entity) {
      continue;
    }

    for (const auto* effect : entity->GetActiveEffects()) {
      if (!effect->is_active) {
        continue;
      }

      if (effect->trigger == event && effect->on_event) {
        logger->debug("Triggering {} effect from {} for {}", effect->name,
                      effect->source, entity->GetName());
        effect->on_event(context);
      }
    }
  }
}

}  // namespace combat
}  // namespace internal

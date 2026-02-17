// -----------------------------------------------------------------------------
// | @file      event_manager.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/combat/event_manager.h"

#include "internal/entities/entity.h"
#include "internal/logging/log_manager.h"
#include "internal/logging/logger.h"

namespace internal {
namespace combat {

void EventManager::Emit(CombatEvent event,
                        std::shared_ptr<EventContext> context) {
  if (!context) {
    return;
  }

  auto logger = logging::LogManager::GetLogger("events");

  auto entities = context->GetInvolvedEntities();

  for (const auto& entity : entities) {
    if (!entity) continue;
    
    for (const auto& effect : entity->GetActiveEffects()) {
      if (!effect.is_active) continue;
      
      if (effect.trigger == event && effect.on_event) {
        logger->Debug("Triggering {} effect from {} for {}", 
                     effect.name, effect.source, entity->GetName());
        effect.on_event(context);
      }
    }
  }
}

} // namespace combat
} // namespace internal

// -----------------------------------------------------------------------------
// | @file      event_manager.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/combat/event_manager.h"

#include "internal/combat/attack.h"
#include "internal/items/enchantment.h"
#include "internal/logging/log_manager.h"
#include "internal/logging/logger.h"

namespace internal {
namespace combat {

void EventManager::Emit(CombatEvent event,
                        std::shared_ptr<CombatContext> context) {
  if (!context || context->results.empty()) {
    return;
  }

  auto& current_result = context->results.back();
  if (!current_result.attack || !current_result.attack->weapon) {
    return;
  }

  auto logger = logging::LogManager::GetLogger("events");

  for (const auto& enchantment : current_result.attack->weapon->enchantments) {
    for (const auto& effect : enchantment.effects) {
      if (effect.trigger == event) {
        logger->Debug("Triggering {} effect", effect.name);
        effect.on(context);
      }
    }
  }
}

}  // namespace combat
}  // namespace internal

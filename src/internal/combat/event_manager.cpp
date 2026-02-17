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

  std::vector<std::shared_ptr<entities::Entity>> relevant_entities;

  if (auto dmg_ctx = std::dynamic_pointer_cast<DamageContext>(context)) {
    switch (event) {
      case CombatEvent::TakeDamage:
        if (dmg_ctx->target)
          relevant_entities.push_back(dmg_ctx->target);
        break;
      case CombatEvent::DealDamage:
        if (dmg_ctx->attacker)
          relevant_entities.push_back(dmg_ctx->attacker);
        break;
      case CombatEvent::Kill:
        if (dmg_ctx->attacker)
          relevant_entities.push_back(dmg_ctx->attacker);
        break;
      default:
        relevant_entities = entities;
        break;
    }
  } else if (auto heal_ctx = std::dynamic_pointer_cast<HealContext>(context)) {
    if (event == CombatEvent::Heal) {
      if (heal_ctx->target)
        relevant_entities.push_back(heal_ctx->target);
    } else {
      relevant_entities = entities;
    }
  } else if (auto hit_ctx = std::dynamic_pointer_cast<HitContext>(context)) {
    if (event == CombatEvent::Hit) {
      if (hit_ctx->attacker)
        relevant_entities.push_back(hit_ctx->attacker);
    } else {
      relevant_entities = entities;
    }
  } else {
    relevant_entities = entities;
  }

  for (const auto& entity : relevant_entities) {
    if (!entity)
      continue;

    for (const auto& effect : entity->GetActiveEffects()) {
      if (!effect.is_active)
        continue;

      if (effect.trigger == event && effect.on_event) {
        logger->Debug("Triggering {} effect from {} for {}", effect.name,
                      effect.source, entity->GetName());
        effect.on_event(context);
      }
    }
  }
}

}  // namespace combat
}  // namespace internal

// -----------------------------------------------------------------------------
// | @file      event_manager.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/combat/event_manager.h"

#include "internal/combat/combat_context.h"
#include "internal/combat/damage_modification.h"
#include "internal/entities/i_entity.h"
#include "internal/logging/log_manager.h"

namespace internal {
namespace combat {

std::vector<DamageModification> EmitCombatEvent(
    CombatEvent event, CombatEventContext* context) {
  if (!context) {
    return {};
  }

  std::vector<DamageModification> modifications;

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

      if (effect->trigger != event) {
        continue;
      }

      logger->debug("Triggering {} effect from {} for {}", effect->name,
                    effect->source, entity->GetName());

      if (effect->on_event) {
        effect->on_event(*context);
      }

      if (effect->on_damage) {
        modifications.push_back(effect->on_damage(*context));
      }
    }
  }

  return modifications;
}

}  // namespace combat
}  // namespace internal

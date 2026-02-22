// -----------------------------------------------------------------------------
// | @file      director.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/engine/director.h"

#include "internal/combat/event_manager.h"
#include "internal/entities/i_entity.h"
#include "internal/logging/log_manager.h"

namespace ettes {
namespace engine {

Director::Director(Encounter* encounter, CombatEngine* engine)
    : encounter_(encounter),
      engine_(engine),
      logger_(logging::LogManager::GetLogger("director")) {
}

Director::~Director() = default;

void Director::RunEncounter() {
  while (!encounter_->IsOver()) {
    attacks_this_round_.clear();
    for (const auto& entity : encounter_->GetSideA()) {
      if (encounter_->IsOver()) {
        break;
      }
      if (!entity || !entity->IsAlive()) {
        continue;
      }
      RunTurn(entity.get());
    }
    for (const auto& entity : encounter_->GetSideB()) {
      if (encounter_->IsOver()) {
        break;
      }
      if (!entity || !entity->IsAlive()) {
        continue;
      }
      RunTurn(entity.get());
    }
  }

  logger_->info("--- Encounter over ---");
  for (const auto& e : encounter_->GetSideA()) {
    logger_->info("  [A] {} - {} HP", e->GetName(),
                  e->GetCurrentStats().base_stats.hp);
  }
  for (const auto& e : encounter_->GetSideB()) {
    logger_->info("  [B] {} - {} HP", e->GetName(),
                  e->GetCurrentStats().base_stats.hp);
  }
}

void Director::RunTurn(entities::IEntity* entity) {
  if (!entity || !entity->IsAlive()) {
    return;
  }

  auto target = SelectTarget(entity);
  if (!target) {
    logger_->info("{} has no living targets, skipping turn", entity->GetName());
    return;
  }

  // TODO(zyrotot): Add positioning and range, removing the arbitrary adjacency
  // limit.
  if (attacks_this_round_[target] >= kMaxAdjacentAttackers) {
    logger_->info("{} cannot attack {} - adjacency limit of {} reached",
                  entity->GetName(), target->GetName(), kMaxAdjacentAttackers);
    return;
  }
  attacks_this_round_[target]++;

  logger_->info("--- {}'s turn ---", entity->GetName());

  combat::CombatEventContext context;
  context.source = entity;
  combat::EmitCombatEvent(combat::CombatEvent::TurnStart, &context);

  engine_->QueueAttack({entity, target, 0});

  engine_->Flush(this);
}

void Director::QueueAttack(combat::QueuedAttack attack) {
  if (!attack.target || !attack.target->IsAlive()) {
    if (attack.attacker) {
      attack.target = SelectTarget(attack.attacker);
    }
  }

  if (!attack.target) {
    logger_->info("No living target available for queued attack by {}, dropped",
                  attack.attacker ? attack.attacker->GetName() : "unknown");
    return;
  }

  if (attacks_this_round_[attack.target] >= kMaxAdjacentAttackers) {
    logger_->info(
        "Queued attack by {} on {} dropped - adjacency limit of {} reached",
        attack.attacker ? attack.attacker->GetName() : "unknown",
        attack.target->GetName(), kMaxAdjacentAttackers);
    return;
  }
  attacks_this_round_[attack.target]++;

  engine_->QueueAttack(std::move(attack));
}

void Director::NotifyEntityDied(entities::IEntity* entity) {
  encounter_->NotifyDeath(entity);
}

entities::IEntity* Director::SelectTarget(entities::IEntity* attacker) const {
  return encounter_->GetFirstLivingEnemyOf(attacker);
}

}  // namespace engine
}  // namespace ettes

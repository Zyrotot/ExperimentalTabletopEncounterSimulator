// -----------------------------------------------------------------------------
// | @file      ai_director.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/engine/ai_director.h"

#include "internal/entities/entity.h"
#include "internal/logging/log_manager.h"
#include "internal/logging/logger.h"

namespace internal {
namespace engine {

AIDirector::AIDirector(Encounter* encounter, CombatEngine* engine)
    : encounter_(encounter),
      engine_(engine),
      logger_(logging::LogManager::GetLogger("director")) {
}

AIDirector::~AIDirector() {
}

void AIDirector::RunEncounter() {
  while (!encounter_->IsOver()) {
    for (const auto& entity : encounter_->GetSideA()) {
      if (encounter_->IsOver()) break;
      if (!entity || !entity->IsAlive()) continue;
      RunTurn(entity);
    }
    for (const auto& entity : encounter_->GetSideB()) {
      if (encounter_->IsOver()) break;
      if (!entity || !entity->IsAlive()) continue;
      RunTurn(entity);
    }
  }

  logger_->Info("--- Encounter over ---");
  for (const auto& e : encounter_->GetSideA()) {
    logger_->Info("  [A] {} - {} HP", e->GetName(),
                  e->GetCurrentStats().base_stats.hp);
  }
  for (const auto& e : encounter_->GetSideB()) {
    logger_->Info("  [B] {} - {} HP", e->GetName(),
                  e->GetCurrentStats().base_stats.hp);
  }
}

void AIDirector::RunTurn(std::shared_ptr<entities::Entity> entity) {
  if (!entity || !entity->IsAlive()) {
    return;
  }

  auto target = SelectTarget(entity);
  if (!target) {
    logger_->Info("{} has no living targets, skipping turn", entity->GetName());
    return;
  }

  logger_->Info("--- {}'s turn ---", entity->GetName());
  engine_->QueueAttack(
      {entity, target, 0});  // TODO(zyrotot): support multiple attack sequences

  engine_->Flush(this);
}

void AIDirector::QueueAttack(combat::QueuedAttack attack) {
  if (!attack.target || !attack.target->IsAlive()) {
    if (attack.attacker) {
      attack.target = SelectTarget(attack.attacker);
    }
  }

  if (!attack.target) {
    logger_->Info("No living target available for queued attack by {}, dropped",
                  attack.attacker ? attack.attacker->GetName() : "unknown");
    return;
  }

  engine_->QueueAttack(std::move(attack));
}

std::shared_ptr<entities::Entity> AIDirector::SelectTarget(
    std::shared_ptr<entities::Entity> attacker) const {
  return encounter_->GetFirstLivingEnemyOf(attacker.get());
}

}  // namespace engine
}  // namespace internal

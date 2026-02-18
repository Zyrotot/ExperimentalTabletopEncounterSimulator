// -----------------------------------------------------------------------------
// | @file      combat_engine.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/engine/combat_engine.h"

#include "internal/dice_rolls/roller.h"
#include "internal/entities/entity.h"
#include "internal/logging/log_manager.h"
#include "internal/logging/logger.h"
#include "internal/resolver/attack_resolver.h"
#include "internal/resolver/damage_resolver.h"

namespace internal {
namespace engine {

CombatEngine::CombatEngine(std::shared_ptr<dice_rolls::Roller> roller)
    : roller_(roller),
      logger_(logging::LogManager::GetLogger("engine")) {
}

CombatEngine::~CombatEngine() {
}

void CombatEngine::QueueAttack(combat::QueuedAttack attack) {
  attack_queue_.push_back(std::move(attack));
}

std::vector<std::shared_ptr<combat::CombatEventContext>> CombatEngine::Flush(
    combat::IAttackQueue* context_queue) {
  std::vector<std::shared_ptr<combat::CombatEventContext>> out_contexts;

  while (!attack_queue_.empty()) {
    combat::QueuedAttack queued = std::move(attack_queue_.front());
    attack_queue_.pop_front();

    if (!queued.attacker || !queued.target) {
      logger_->Warning("Skipping queued attack with null attacker or target");
      continue;
    }

    logger_->Info("{} attacks {}", queued.attacker->GetName(),
                  queued.target->GetName());
    ProcessAttack(queued.attacker, queued.target, queued.attack_sequence_index,
                  context_queue, &out_contexts);
  }

  return out_contexts;
}

void CombatEngine::ProcessAttack(
    std::shared_ptr<entities::Entity> attacker,
    std::shared_ptr<entities::Entity> defender, int attack_sequence_index,
    combat::IAttackQueue* context_queue,
    std::vector<std::shared_ptr<combat::CombatEventContext>>* out_contexts) {
  auto resolver = std::make_shared<resolver::AttackResolver>(
      attacker, defender, attack_sequence_index, roller_);

  auto context = std::make_shared<combat::CombatEventContext>();
  context->source = attacker;
  context->target = defender;
  context->roller = roller_;
  context->attack_queue = context_queue;

  auto damage_resolver = std::make_shared<resolver::DamageResolver>(context);

  const auto& sequence = attacker->GetAttackSequence(attack_sequence_index);
  for (size_t i = 0; i < sequence.attacks.size(); ++i) {
    resolver->ResolveSingleAttack(i, context);
    damage_resolver->ApplySingleAttack(context->results.size() - 1);
  }

  out_contexts->push_back(context);
}

}  // namespace engine
}  // namespace internal

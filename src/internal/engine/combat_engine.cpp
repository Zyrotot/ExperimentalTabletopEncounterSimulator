// -----------------------------------------------------------------------------
// | @file      combat_engine.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/engine/combat_engine.h"

#include "internal/dice_rolls/roller.h"
#include "internal/entities/i_entity.h"
#include "internal/logging/log_manager.h"
#include "internal/resolver/attack_resolver.h"
#include "internal/resolver/damage_resolver.h"

namespace ettes {
namespace engine {

CombatEngine::CombatEngine(std::shared_ptr<dice_rolls::Roller> roller)
    : roller_(roller), logger_(logging::LogManager::GetLogger("engine")) {
}

CombatEngine::~CombatEngine() = default;

void CombatEngine::QueueAttack(combat::QueuedAttack attack) {
  attack_queue_.push_back(std::move(attack));
}

std::vector<std::unique_ptr<combat::CombatEventContext>> CombatEngine::Flush(
    combat::IAttackQueue* context_queue) {
  std::vector<std::unique_ptr<combat::CombatEventContext>> out_contexts;

  while (!attack_queue_.empty()) {
    combat::QueuedAttack queued = std::move(attack_queue_.front());
    attack_queue_.pop_front();

    if (!queued.attacker || !queued.target) {
      logger_->warn("Skipping queued attack with null attacker or target");
      continue;
    }

    logger_->info("{} attacks {}", queued.attacker->GetName(),
                  queued.target->GetName());
    ProcessAttack(queued.attacker, queued.target, queued.attack_sequence_index,
                  context_queue, &out_contexts);
  }

  return out_contexts;
}

void CombatEngine::ProcessAttack(
    std::shared_ptr<entities::IEntity> attacker,
    std::shared_ptr<entities::IEntity> defender, int attack_sequence_index,
    combat::IAttackQueue* context_queue,
    std::vector<std::unique_ptr<combat::CombatEventContext>>* out_contexts) {
  auto context = std::make_unique<combat::CombatEventContext>();
  context->source = attacker;
  context->target = defender;
  context->roller = roller_.get();
  context->attack_queue = context_queue;

  const auto& sequence = attacker->GetAttackSequence(attack_sequence_index);

  auto resolver =
      std::make_shared<resolver::AttackResolver>(sequence, context.get());
  auto damage_resolver =
      std::make_shared<resolver::DamageResolver>(context.get());

  for (size_t i = 0; i < sequence.attacks.size(); ++i) {
    resolver->ResolveSingleAttack(i);
    damage_resolver->ApplySingleAttack(context->results.size() - 1);
  }

  out_contexts->push_back(std::move(context));
}

}  // namespace engine
}  // namespace ettes

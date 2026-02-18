// -----------------------------------------------------------------------------
// | @file      combat_engine.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENGINE_COMBAT_ENGINE_H_
#define SRC_INTERNAL_ENGINE_COMBAT_ENGINE_H_

#include <deque>
#include <memory>
#include <vector>

#include "internal/combat/combat_events.h"

namespace internal {

namespace entities {
class Entity;
}  // namespace entities

namespace dice_rolls {
class Roller;
}  // namespace dice_rolls

namespace logging {
class Logger;
}  // namespace logging

namespace engine {

class CombatEngine {
 public:
  explicit CombatEngine(std::shared_ptr<dice_rolls::Roller> roller);
  ~CombatEngine();

  void QueueAttack(combat::QueuedAttack attack);

  std::vector<std::shared_ptr<combat::CombatEventContext>> Flush(
      combat::IAttackQueue* context_queue);

 private:
  void ProcessAttack(
      std::shared_ptr<entities::Entity> attacker,
      std::shared_ptr<entities::Entity> defender, int attack_sequence_index,
      combat::IAttackQueue* context_queue,
      std::vector<std::shared_ptr<combat::CombatEventContext>>* out_contexts);

  std::shared_ptr<dice_rolls::Roller> roller_;
  std::shared_ptr<logging::Logger> logger_;
  std::deque<combat::QueuedAttack> attack_queue_;
};

}  // namespace engine
}  // namespace internal

#endif  // SRC_INTERNAL_ENGINE_COMBAT_ENGINE_H_

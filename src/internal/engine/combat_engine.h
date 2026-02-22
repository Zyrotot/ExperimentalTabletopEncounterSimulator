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

#include "internal/combat/attack_queue.h"
#include "internal/combat/attack_result.h"
#include "internal/combat/combat_context.h"
#include "internal/logging/logger.h"

namespace ettes {

namespace entities {
class IEntity;
}  // namespace entities

namespace dice_rolls {
class Roller;
}  // namespace dice_rolls

namespace engine {

class CombatEngine {
 public:
  explicit CombatEngine(std::shared_ptr<dice_rolls::Roller> roller);
  ~CombatEngine();

  void QueueAttack(combat::QueuedAttack attack);

  std::vector<std::unique_ptr<combat::CombatEventContext>> Flush(
      combat::IAttackQueue* context_queue);

 private:
  void ProcessAttack(
      std::shared_ptr<entities::IEntity> attacker,
      std::shared_ptr<entities::IEntity> defender, int attack_sequence_index,
      combat::IAttackQueue* context_queue,
      std::vector<std::unique_ptr<combat::CombatEventContext>>* out_contexts);

  std::shared_ptr<dice_rolls::Roller> roller_;
  logging::Logger* logger_;
  std::deque<combat::QueuedAttack> attack_queue_;
};

}  // namespace engine
}  // namespace ettes

#endif  // SRC_INTERNAL_ENGINE_COMBAT_ENGINE_H_

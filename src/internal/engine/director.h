// -----------------------------------------------------------------------------
// | @file      director.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENGINE_DIRECTOR_H_
#define SRC_INTERNAL_ENGINE_DIRECTOR_H_

#include <memory>
#include <unordered_map>

#include "internal/combat/attack_queue.h"
#include "internal/engine/combat_engine.h"
#include "internal/engine/encounter.h"
#include "internal/logging/logger.h"

namespace ettes {

namespace engine {

class Director : public combat::IAttackQueue {
 public:
  Director(Encounter* encounter, CombatEngine* engine);
  ~Director() override;

  void RunEncounter();

  void RunTurn(std::shared_ptr<entities::IEntity> entity);

  void QueueAttack(combat::QueuedAttack attack) override;

 private:
  std::shared_ptr<entities::IEntity> SelectTarget(
      std::shared_ptr<entities::IEntity> attacker) const;

  static constexpr int kMaxAdjacentAttackers = 8;

  Encounter* encounter_;
  CombatEngine* engine_;
  logging::Logger* logger_;
  std::unordered_map<const entities::IEntity*, int> attacks_this_round_;
};

}  // namespace engine
}  // namespace ettes

#endif  // SRC_INTERNAL_ENGINE_DIRECTOR_H_

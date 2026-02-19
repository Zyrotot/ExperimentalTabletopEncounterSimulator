// -----------------------------------------------------------------------------
// | @file      director.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENGINE_DIRECTOR_H_
#define SRC_INTERNAL_ENGINE_DIRECTOR_H_

#include <memory>
#include <unordered_map>

#include "internal/combat/combat_events.h"
#include "internal/engine/combat_engine.h"
#include "internal/engine/encounter.h"

namespace internal {

namespace logging {
class Logger;
}  // namespace logging

namespace engine {

class Director : public combat::IAttackQueue {
 public:
  Director(Encounter* encounter, CombatEngine* engine);
  ~Director() override;

  void RunEncounter();

  void RunTurn(std::shared_ptr<entities::Entity> entity);

  void QueueAttack(combat::QueuedAttack attack) override;

 private:
  std::shared_ptr<entities::Entity> SelectTarget(
      std::shared_ptr<entities::Entity> attacker) const;

  static constexpr int kMaxAdjacentAttackers = 8;

  Encounter* encounter_;
  CombatEngine* engine_;
  std::shared_ptr<logging::Logger> logger_;
  std::unordered_map<const entities::Entity*, int> attacks_this_round_;
};

}  // namespace engine
}  // namespace internal

#endif  // SRC_INTERNAL_ENGINE_DIRECTOR_H_

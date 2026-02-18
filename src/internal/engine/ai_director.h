// -----------------------------------------------------------------------------
// | @file      ai_director.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENGINE_AI_DIRECTOR_H_
#define SRC_INTERNAL_ENGINE_AI_DIRECTOR_H_

#include <memory>

#include "internal/combat/combat_events.h"
#include "internal/engine/combat_engine.h"
#include "internal/engine/encounter.h"

namespace internal {

namespace logging {
class Logger;
}  // namespace logging

namespace engine {

class AIDirector : public combat::IAttackQueue {
 public:
  AIDirector(Encounter* encounter, CombatEngine* engine);
  ~AIDirector() override;

  void RunEncounter();

  void RunTurn(std::shared_ptr<entities::Entity> entity);

  void QueueAttack(combat::QueuedAttack attack) override;

 private:
  std::shared_ptr<entities::Entity> SelectTarget(
      std::shared_ptr<entities::Entity> attacker) const;

  Encounter* encounter_;
  CombatEngine* engine_;
  std::shared_ptr<logging::Logger> logger_;
};

}  // namespace engine
}  // namespace internal

#endif  // SRC_INTERNAL_ENGINE_AI_DIRECTOR_H_

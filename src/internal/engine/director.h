// -----------------------------------------------------------------------------
// | @file      director.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENGINE_DIRECTOR_H_
#define SRC_INTERNAL_ENGINE_DIRECTOR_H_

#include <memory>
#include <vector>

#include "internal/combat/attack_queue.h"
#include "internal/engine/combat_engine.h"
#include "internal/engine/encounter.h"
#include "internal/logging/logger.h"
#include "internal/positioning/grid_pos.h"
#include "internal/positioning/movement_resolver.h"
#include "internal/positioning/position_map.h"

namespace ettes {

namespace engine {

class Director : public combat::IAttackQueue {
 public:
  Director(Encounter* encounter, CombatEngine* engine,
           positioning::DiagonalMode diagonal_mode =
               positioning::DiagonalMode::Alternating);
  ~Director() override;

  void RunEncounter();

  void RunTurn(entities::IEntity* entity, int attack_sequence_index = 0);

  void QueueAttack(combat::QueuedAttack attack) override;
  void NotifyEntityDied(entities::IEntity* entity) override;

  const positioning::PositionMap* GetPositionMap() const;

 private:
  void InitPositionMap();
  void LogGrid() const;

  entities::IEntity* SelectTarget(entities::IEntity* attacker) const;
  double GetMaxAttackRange(const entities::IEntity* entity,
                           int attack_index) const;
  void ExecuteTurn(entities::IEntity* entity, int attack_sequence_index,
                   const std::vector<uint32_t>& ally_ids);

  Encounter* encounter_;
  CombatEngine* engine_;
  logging::Logger* logger_;

  positioning::DiagonalMode diagonal_mode_;
  std::unique_ptr<positioning::PositionMap> position_map_;
  std::unique_ptr<positioning::MovementResolver> movement_resolver_;
};

}  // namespace engine
}  // namespace ettes

#endif  // SRC_INTERNAL_ENGINE_DIRECTOR_H_

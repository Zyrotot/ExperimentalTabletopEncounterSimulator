// -----------------------------------------------------------------------------
// | @file      movement_resolver.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_POSITIONING_MOVEMENT_RESOLVER_H_
#define SRC_INTERNAL_POSITIONING_MOVEMENT_RESOLVER_H_

#include <vector>

#include "internal/positioning/grid_pos.h"
#include "internal/positioning/position_map.h"

namespace ettes {

namespace entities {
class IEntity;
}  // namespace entities

namespace positioning {

class MovementResolver {
 public:
  explicit MovementResolver(PositionMap* position_map);

  std::vector<GridPos> PlanMove(const entities::IEntity* entity,
                                GridPos target,
                                int remaining_speed_cells) const;

  void ApplyMove(const entities::IEntity* entity, GridPos target);

  GridPos FindClosestReachableCell(const entities::IEntity* entity,
                                   const entities::IEntity* target,
                                   int remaining_speed_cells) const;

 private:
  PositionMap* position_map_;
};

}  // namespace positioning
}  // namespace ettes

#endif  // SRC_INTERNAL_POSITIONING_MOVEMENT_RESOLVER_H_

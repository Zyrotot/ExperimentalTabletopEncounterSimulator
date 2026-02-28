// -----------------------------------------------------------------------------
// | @file      movement_resolver.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/positioning/movement_resolver.h"

#include <cmath>

#include "internal/entities/i_entity.h"

namespace ettes {
namespace positioning {

MovementResolver::MovementResolver(PositionMap* position_map)
    : position_map_(position_map) {
}

std::vector<GridPos> MovementResolver::PlanMove(
    const entities::IEntity* entity, GridPos target,
    int remaining_speed_cells) const {
  std::vector<GridPos> path;
  if (!entity || !position_map_->HasEntity(entity)) {
    return path;
  }

  GridPos current = position_map_->GetAnchor(entity);
  EntitySize size = position_map_->GetSize(entity);
  DiagonalMode mode = position_map_->GetDiagonalMode();

  int total_cost = 0;
  int diagonal_count = 0;

  while (current != target) {
    int dx_sign = 0;
    if (target.x > current.x) {
      dx_sign = 1;
    } else if (target.x < current.x) {
      dx_sign = -1;
    }

    int dy_sign = 0;
    if (target.y > current.y) {
      dy_sign = 1;
    } else if (target.y < current.y) {
      dy_sign = -1;
    }

    bool is_diagonal = (dx_sign != 0 && dy_sign != 0);
    int step_cost = 1;

    if (is_diagonal) {
      switch (mode) {
        case DiagonalMode::Simple:
          step_cost = 1;
          break;
        case DiagonalMode::Alternating:
          diagonal_count++;
          if (diagonal_count % 2 == 0) {
            step_cost = 2;
          } else {
            step_cost = 1;
          }
          break;
        case DiagonalMode::Full:
          step_cost = 2;
          break;
      }
    }

    if (total_cost + step_cost > remaining_speed_cells) {
      break;
    }

    GridPos next{current.x + dx_sign, current.y + dy_sign};

    if (!position_map_->IsAreaFree(next, size, entity)) {
      if (is_diagonal) {
        GridPos cardinal_x{current.x + dx_sign, current.y};
        GridPos cardinal_y{current.x, current.y + dy_sign};

        if (position_map_->IsAreaFree(cardinal_x, size, entity)) {
          next = cardinal_x;
          diagonal_count--;
          step_cost = 1;
        } else if (position_map_->IsAreaFree(cardinal_y, size, entity)) {
          next = cardinal_y;
          diagonal_count--;
          step_cost = 1;
        } else {
          break;
        }
      } else {
        break;
      }
    }

    total_cost += step_cost;
    current = next;
    path.push_back(current);
  }

  return path;
}

void MovementResolver::ApplyMove(const entities::IEntity* entity,
                                 GridPos target) {
  position_map_->Move(entity, target);
}

GridPos MovementResolver::FindClosestReachableCell(
    const entities::IEntity* entity, const entities::IEntity* target,
    int remaining_speed_cells) const {
  if (!entity || !target) {
    return {0, 0};
  }
  if (!position_map_->HasEntity(entity) ||
      !position_map_->HasEntity(target)) {
    return position_map_->GetAnchor(entity);
  }

  GridPos target_anchor = position_map_->GetAnchor(target);
  EntitySize target_size = position_map_->GetSize(target);
  int target_foot = FootprintCells(target_size);

  EntitySize entity_size = position_map_->GetSize(entity);
  GridPos entity_anchor = position_map_->GetAnchor(entity);

  GridPos best = entity_anchor;
  int best_dist = position_map_->GetDistanceCells(entity, target);
  int best_travel = 0;

  for (int tx = target_anchor.x - 1;
       tx <= target_anchor.x + target_foot; ++tx) {
    for (int ty = target_anchor.y - 1;
         ty <= target_anchor.y + target_foot; ++ty) {
      GridPos candidate{tx, ty};
      if (candidate.x < 0 || candidate.x >= position_map_->GetWidth() ||
          candidate.y < 0 || candidate.y >= position_map_->GetHeight()) {
        continue;
      }

      auto path = PlanMove(entity, candidate, remaining_speed_cells);
      if (path.empty() && candidate != entity_anchor) {
        continue;
      }

      GridPos endpoint = path.empty() ? entity_anchor : path.back();

      int a_foot = FootprintCells(entity_size);
      int a_max_x = endpoint.x + a_foot - 1;
      int a_max_y = endpoint.y + a_foot - 1;
      int b_max_x = target_anchor.x + target_foot - 1;
      int b_max_y = target_anchor.y + target_foot - 1;

      int ddx = 0;
      if (a_max_x < target_anchor.x) {
        ddx = target_anchor.x - a_max_x;
      } else if (b_max_x < endpoint.x) {
        ddx = endpoint.x - b_max_x;
      }

      int ddy = 0;
      if (a_max_y < target_anchor.y) {
        ddy = target_anchor.y - a_max_y;
      } else if (b_max_y < endpoint.y) {
        ddy = endpoint.y - b_max_y;
      }

      GridPos origin{0, 0};
      GridPos delta{ddx, ddy};
      int dist = DiagonalDistance(origin, delta,
                                  position_map_->GetDiagonalMode());

      int travel = DiagonalDistance(entity_anchor, endpoint,
                                    position_map_->GetDiagonalMode());
      int deviation = std::abs(endpoint.x - entity_anchor.x) +
                      std::abs(endpoint.y - entity_anchor.y);

      int best_deviation = std::abs(best.x - entity_anchor.x) +
                           std::abs(best.y - entity_anchor.y);

      bool is_better = (dist < best_dist) ||
                       (dist == best_dist && travel < best_travel) ||
                       (dist == best_dist && travel == best_travel &&
                        deviation < best_deviation);
      if (is_better) {
        best_dist = dist;
        best = endpoint;
        best_travel = travel;
      }
    }
  }

  return best;
}

}  // namespace positioning
}  // namespace ettes

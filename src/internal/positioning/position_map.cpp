// -----------------------------------------------------------------------------
// | @file      position_map.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/positioning/position_map.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "internal/entities/i_entity.h"

namespace ettes {
namespace positioning {

PositionMap::PositionMap(int width, int height, DiagonalMode mode)
    : width_(width), height_(height), diagonal_mode_(mode) {
}

int PositionMap::GetWidth() const {
  return width_;
}

int PositionMap::GetHeight() const {
  return height_;
}

DiagonalMode PositionMap::GetDiagonalMode() const {
  return diagonal_mode_;
}

void PositionMap::Place(const entities::IEntity* entity, GridPos anchor,
                        EntitySize size) {
  if (!entity) {
    return;
  }
  uint32_t id = entity->GetId();
  placements_[id] = {anchor, size};
  id_to_entity_[id] = entity;
}

void PositionMap::Move(const entities::IEntity* entity, GridPos new_anchor) {
  if (!entity) {
    return;
  }
  auto it = placements_.find(entity->GetId());
  if (it != placements_.end()) {
    it->second.anchor = new_anchor;
  }
}

void PositionMap::Remove(const entities::IEntity* entity) {
  if (!entity) {
    return;
  }
  uint32_t id = entity->GetId();
  placements_.erase(id);
  id_to_entity_.erase(id);
}

bool PositionMap::HasEntity(const entities::IEntity* entity) const {
  if (!entity) {
    return false;
  }
  return placements_.find(entity->GetId()) != placements_.end();
}

GridPos PositionMap::GetAnchor(const entities::IEntity* entity) const {
  auto it = placements_.find(entity->GetId());
  if (it != placements_.end()) {
    return it->second.anchor;
  }
  return {-1, -1};
}

EntitySize PositionMap::GetSize(const entities::IEntity* entity) const {
  auto it = placements_.find(entity->GetId());
  if (it != placements_.end()) {
    return it->second.size;
  }
  return EntitySize::Medium;
}

int PositionMap::MinFootprintDistance(const Placement& a,
                                     const Placement& b) const {
  int a_foot = FootprintCells(a.size);
  int b_foot = FootprintCells(b.size);

  int a_min_x = a.anchor.x;
  int a_max_x = a.anchor.x + a_foot - 1;
  int a_min_y = a.anchor.y;
  int a_max_y = a.anchor.y + a_foot - 1;

  int b_min_x = b.anchor.x;
  int b_max_x = b.anchor.x + b_foot - 1;
  int b_min_y = b.anchor.y;
  int b_max_y = b.anchor.y + b_foot - 1;

  int dx = 0;
  if (a_max_x < b_min_x) {
    dx = b_min_x - a_max_x;
  } else if (b_max_x < a_min_x) {
    dx = a_min_x - b_max_x;
  }

  int dy = 0;
  if (a_max_y < b_min_y) {
    dy = b_min_y - a_max_y;
  } else if (b_max_y < a_min_y) {
    dy = a_min_y - b_max_y;
  }

  GridPos origin{0, 0};
  GridPos delta{dx, dy};
  return DiagonalDistance(origin, delta, diagonal_mode_);
}

double PositionMap::GetDistanceMeters(const entities::IEntity* a,
                                      const entities::IEntity* b) const {
  return static_cast<double>(GetDistanceCells(a, b)) * kCellSizeMeters;
}

int PositionMap::GetDistanceCells(const entities::IEntity* a,
                                  const entities::IEntity* b) const {
  if (!a || !b) {
    return std::numeric_limits<int>::max();
  }
  auto it_a = placements_.find(a->GetId());
  auto it_b = placements_.find(b->GetId());
  if (it_a == placements_.end() || it_b == placements_.end()) {
    return std::numeric_limits<int>::max();
  }
  return MinFootprintDistance(it_a->second, it_b->second);
}

bool PositionMap::AreAdjacent(const entities::IEntity* a,
                              const entities::IEntity* b) const {
  return GetDistanceCells(a, b) <= 1;
}

std::vector<const entities::IEntity*> PositionMap::GetEntitiesInRange(
    const entities::IEntity* source, double range_meters) const {
  std::vector<const entities::IEntity*> result;
  if (!source) {
    return result;
  }
  auto src_it = placements_.find(source->GetId());
  if (src_it == placements_.end()) {
    return result;
  }

  for (const auto& [id, entity_ptr] : id_to_entity_) {
    if (id == source->GetId()) {
      continue;
    }
    auto it = placements_.find(id);
    if (it == placements_.end()) {
      continue;
    }
    int cell_dist = MinFootprintDistance(src_it->second, it->second);
    double meters = static_cast<double>(cell_dist) * kCellSizeMeters;
    if (meters <= range_meters) {
      result.push_back(entity_ptr);
    }
  }
  return result;
}

std::vector<const entities::IEntity*> PositionMap::GetAdjacentEntities(
    const entities::IEntity* entity) const {
  return GetEntitiesInRange(entity, kCellSizeMeters);
}

bool PositionMap::IsCellOccupied(GridPos pos) const {
  for (const auto& [id, placement] : placements_) {
    int foot = FootprintCells(placement.size);
    if (pos.x >= placement.anchor.x &&
        pos.x < placement.anchor.x + foot &&
        pos.y >= placement.anchor.y &&
        pos.y < placement.anchor.y + foot) {
      return true;
    }
  }
  return false;
}

bool PositionMap::IsAreaFree(GridPos anchor, EntitySize size,
                             const entities::IEntity* ignore) const {
  int foot = FootprintCells(size);
  uint32_t ignore_id = ignore ? ignore->GetId() : 0;
  bool has_ignore = (ignore != nullptr);

  for (int dx = 0; dx < foot; ++dx) {
    for (int dy = 0; dy < foot; ++dy) {
      GridPos cell{anchor.x + dx, anchor.y + dy};
      if (cell.x < 0 || cell.x >= width_ || cell.y < 0 ||
          cell.y >= height_) {
        return false;
      }
      for (const auto& [id, placement] : placements_) {
        if (has_ignore && id == ignore_id) {
          continue;
        }
        int pfoot = FootprintCells(placement.size);
        if (cell.x >= placement.anchor.x &&
            cell.x < placement.anchor.x + pfoot &&
            cell.y >= placement.anchor.y &&
            cell.y < placement.anchor.y + pfoot) {
          return false;
        }
      }
    }
  }
  return true;
}

}  // namespace positioning
}  // namespace ettes

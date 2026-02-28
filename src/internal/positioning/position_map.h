// -----------------------------------------------------------------------------
// | @file      position_map.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_POSITIONING_POSITION_MAP_H_
#define SRC_INTERNAL_POSITIONING_POSITION_MAP_H_

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "internal/positioning/entity_size.h"
#include "internal/positioning/grid_pos.h"

namespace ettes {

namespace entities {
class IEntity;
}  // namespace entities

namespace positioning {

struct Placement {
  GridPos anchor;
  EntitySize size = EntitySize::Medium;
};

class PositionMap {
 public:
  PositionMap(int width, int height,
              DiagonalMode mode = DiagonalMode::Alternating);

  int GetWidth() const;
  int GetHeight() const;
  DiagonalMode GetDiagonalMode() const;

  void Place(const entities::IEntity* entity, GridPos anchor, EntitySize size);
  void Move(const entities::IEntity* entity, GridPos new_anchor);
  void Remove(const entities::IEntity* entity);

  bool HasEntity(const entities::IEntity* entity) const;
  GridPos GetAnchor(const entities::IEntity* entity) const;
  EntitySize GetSize(const entities::IEntity* entity) const;

  double GetDistanceMeters(const entities::IEntity* a,
                           const entities::IEntity* b) const;
  int GetDistanceCells(const entities::IEntity* a,
                       const entities::IEntity* b) const;

  bool AreAdjacent(const entities::IEntity* a,
                   const entities::IEntity* b) const;

  std::vector<const entities::IEntity*> GetEntitiesInRange(
      const entities::IEntity* source, double range_meters) const;

  std::vector<const entities::IEntity*> GetAdjacentEntities(
      const entities::IEntity* entity) const;

  bool IsCellOccupied(GridPos pos) const;
  bool IsAreaFree(GridPos anchor, EntitySize size,
                  const entities::IEntity* ignore = nullptr) const;
  const entities::IEntity* GetEntityBlockingArea(
      GridPos anchor, EntitySize size,
      const entities::IEntity* ignore = nullptr) const;

 private:
  int MinFootprintDistance(const Placement& a, const Placement& b) const;

  int width_;
  int height_;
  DiagonalMode diagonal_mode_;
  std::unordered_map<uint32_t, Placement> placements_;
  std::unordered_map<uint32_t, const entities::IEntity*> id_to_entity_;
};

}  // namespace positioning
}  // namespace ettes

#endif  // SRC_INTERNAL_POSITIONING_POSITION_MAP_H_

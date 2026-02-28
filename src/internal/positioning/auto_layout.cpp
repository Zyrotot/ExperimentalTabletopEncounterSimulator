// -----------------------------------------------------------------------------
// | @file      auto_layout.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/positioning/auto_layout.h"

#include <algorithm>
#include <cmath>

#include "internal/entities/i_entity.h"

namespace ettes {
namespace positioning {

int RecommendedGridWidth(
    const std::vector<entities::IEntity*>& side_a,
    const std::vector<entities::IEntity*>& side_b) {
  int max_count = std::max(static_cast<int>(side_a.size()),
                           static_cast<int>(side_b.size()));
  int max_foot = 1;
  for (const auto* e : side_a) {
    if (e) {
      max_foot = std::max(max_foot, FootprintCells(e->GetEntitySize()));
    }
  }
  for (const auto* e : side_b) {
    if (e) {
      max_foot = std::max(max_foot, FootprintCells(e->GetEntitySize()));
    }
  }
  return std::max(10, (max_count * (max_foot + 1)) + 2);
}

int RecommendedGridHeight(
    const std::vector<entities::IEntity*>& side_a,
    const std::vector<entities::IEntity*>& side_b) {
  int max_foot = 1;
  for (const auto* e : side_a) {
    if (e) {
      max_foot = std::max(max_foot, FootprintCells(e->GetEntitySize()));
    }
  }
  for (const auto* e : side_b) {
    if (e) {
      max_foot = std::max(max_foot, FootprintCells(e->GetEntitySize()));
    }
  }
  return std::max(10, max_foot * 2 + 8);
}

void AutoLayoutSides(
    PositionMap* map,
    const std::vector<entities::IEntity*>& side_a,
    const std::vector<entities::IEntity*>& side_b) {
  if (!map) {
    return;
  }

  int width = map->GetWidth();
  int height = map->GetHeight();

  int col = 1;
  for (const auto* e : side_a) {
    if (!e) {
      continue;
    }
    int foot = FootprintCells(e->GetEntitySize());
    int row = 1;
    GridPos anchor{col, row};

    while (!map->IsAreaFree(anchor, e->GetEntitySize()) &&
           anchor.x + foot < width) {
      anchor.x++;
    }

    map->Place(e, anchor, e->GetEntitySize());
    col = anchor.x + foot;
  }

  col = 1;
  for (const auto* e : side_b) {
    if (!e) {
      continue;
    }
    int foot = FootprintCells(e->GetEntitySize());
    int row = height - foot - 1;
    GridPos anchor{col, row};

    while (!map->IsAreaFree(anchor, e->GetEntitySize()) &&
           anchor.x + foot < width) {
      anchor.x++;
    }

    map->Place(e, anchor, e->GetEntitySize());
    col = anchor.x + foot;
  }
}

}  // namespace positioning
}  // namespace ettes

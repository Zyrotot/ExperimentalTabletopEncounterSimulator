// -----------------------------------------------------------------------------
// | @file      auto_layout.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_POSITIONING_AUTO_LAYOUT_H_
#define SRC_INTERNAL_POSITIONING_AUTO_LAYOUT_H_

#include <vector>

#include "internal/positioning/position_map.h"

namespace ettes {

namespace entities {
class IEntity;
}  // namespace entities

namespace positioning {

void AutoLayoutSides(
    PositionMap* map,
    const std::vector<entities::IEntity*>& side_a,
    const std::vector<entities::IEntity*>& side_b);

int RecommendedGridWidth(
    const std::vector<entities::IEntity*>& side_a,
    const std::vector<entities::IEntity*>& side_b);

int RecommendedGridHeight(
    const std::vector<entities::IEntity*>& side_a,
    const std::vector<entities::IEntity*>& side_b);

}  // namespace positioning
}  // namespace ettes

#endif  // SRC_INTERNAL_POSITIONING_AUTO_LAYOUT_H_

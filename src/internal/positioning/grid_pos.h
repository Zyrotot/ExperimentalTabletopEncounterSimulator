// -----------------------------------------------------------------------------
// | @file      grid_pos.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_POSITIONING_GRID_POS_H_
#define SRC_INTERNAL_POSITIONING_GRID_POS_H_

#include <cmath>

namespace ettes {
namespace positioning {

constexpr double kCellSizeMeters = 1.5;

enum class DiagonalMode {
  Simple,
  Alternating,
  Full
};

struct GridPos {
  int x = 0;
  int y = 0;

  bool operator==(const GridPos& other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const GridPos& other) const {
    return !(*this == other);
  }
};

int ChebyshevDistance(GridPos a, GridPos b);

int DiagonalDistance(GridPos a, GridPos b, DiagonalMode mode);

double DistanceMeters(GridPos a, GridPos b, DiagonalMode mode);

}  // namespace positioning
}  // namespace ettes

#endif  // SRC_INTERNAL_POSITIONING_GRID_POS_H_

// -----------------------------------------------------------------------------
// | @file      grid_pos.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/positioning/grid_pos.h"

#include <algorithm>
#include <cmath>

namespace ettes {
namespace positioning {

int ChebyshevDistance(GridPos a, GridPos b) {
  return std::max(std::abs(a.x - b.x), std::abs(a.y - b.y));
}

int DiagonalDistance(GridPos a, GridPos b, DiagonalMode mode) {
  int dx = std::abs(a.x - b.x);
  int dy = std::abs(a.y - b.y);
  int cardinal = std::abs(dx - dy);
  int diagonal = std::min(dx, dy);

  switch (mode) {
    case DiagonalMode::Simple:
      return cardinal + diagonal;

    case DiagonalMode::Alternating: {
      int extra = diagonal / 2;
      return cardinal + diagonal + extra;
    }

    case DiagonalMode::Full:
      return cardinal + diagonal * 2;
  }

  return cardinal + diagonal;
}

double DistanceMeters(GridPos a, GridPos b, DiagonalMode mode) {
  return static_cast<double>(DiagonalDistance(a, b, mode)) * kCellSizeMeters;
}

}  // namespace positioning
}  // namespace ettes

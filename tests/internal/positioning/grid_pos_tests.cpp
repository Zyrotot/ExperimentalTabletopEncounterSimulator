// -----------------------------------------------------------------------------
// | @file      grid_pos_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/positioning/grid_pos.h"

namespace ettes {
namespace positioning {

class GridPosTest : public ::testing::Test {};

TEST_F(GridPosTest, EqualityOperator) {
  GridPos a{3, 5};
  GridPos b{3, 5};
  GridPos c{4, 5};

  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST_F(GridPosTest, ChebyshevDistanceSamePoint) {
  EXPECT_EQ(ChebyshevDistance({0, 0}, {0, 0}), 0);
}

TEST_F(GridPosTest, ChebyshevDistanceCardinal) {
  EXPECT_EQ(ChebyshevDistance({0, 0}, {5, 0}), 5);
  EXPECT_EQ(ChebyshevDistance({0, 0}, {0, 3}), 3);
}

TEST_F(GridPosTest, ChebyshevDistanceDiagonal) {
  EXPECT_EQ(ChebyshevDistance({0, 0}, {4, 4}), 4);
  EXPECT_EQ(ChebyshevDistance({0, 0}, {3, 5}), 5);
}

TEST_F(GridPosTest, DiagonalDistanceSimple) {
  EXPECT_EQ(DiagonalDistance({0, 0}, {4, 4}, DiagonalMode::Simple), 4);
  EXPECT_EQ(DiagonalDistance({0, 0}, {3, 0}, DiagonalMode::Simple), 3);
  EXPECT_EQ(DiagonalDistance({0, 0}, {3, 5}, DiagonalMode::Simple), 3 + 2);
}

TEST_F(GridPosTest, DiagonalDistanceAlternating) {
  EXPECT_EQ(DiagonalDistance({0, 0}, {0, 0}, DiagonalMode::Alternating), 0);
  EXPECT_EQ(DiagonalDistance({0, 0}, {3, 0}, DiagonalMode::Alternating), 3);
  EXPECT_EQ(DiagonalDistance({0, 0}, {2, 2}, DiagonalMode::Alternating), 3);
  EXPECT_EQ(DiagonalDistance({0, 0}, {4, 4}, DiagonalMode::Alternating), 6);
}

TEST_F(GridPosTest, DiagonalDistanceFull) {
  EXPECT_EQ(DiagonalDistance({0, 0}, {0, 0}, DiagonalMode::Full), 0);
  EXPECT_EQ(DiagonalDistance({0, 0}, {3, 0}, DiagonalMode::Full), 3);
  EXPECT_EQ(DiagonalDistance({0, 0}, {2, 2}, DiagonalMode::Full), 4);
  EXPECT_EQ(DiagonalDistance({0, 0}, {3, 5}, DiagonalMode::Full), 2 + 3 * 2);
}

TEST_F(GridPosTest, DistanceMetersUsesSquareSize) {
  double dist = DistanceMeters({0, 0}, {2, 0}, DiagonalMode::Simple);
  EXPECT_DOUBLE_EQ(dist, 3.0);
}

TEST_F(GridPosTest, DistanceMetersAlternating) {
  double dist = DistanceMeters({0, 0}, {2, 2}, DiagonalMode::Alternating);
  EXPECT_DOUBLE_EQ(dist, 3.0 * kCellSizeMeters);
}

}  // namespace positioning
}  // namespace ettes

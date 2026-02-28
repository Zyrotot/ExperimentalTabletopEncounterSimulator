// -----------------------------------------------------------------------------
// | @file      movement_resolver_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "internal/positioning/grid_pos.h"
#include "internal/positioning/movement_resolver.h"
#include "tests/mocks/mock_entity.h"

using ::testing::Return;

namespace ettes {
namespace positioning {

class MovementResolverTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockEntity> MakeMockEntity(uint32_t id) {
    auto mock = std::make_unique<MockEntity>();
    ON_CALL(*mock, GetId()).WillByDefault(Return(id));
    ON_CALL(*mock, IsAlive()).WillByDefault(Return(true));
    ON_CALL(*mock, GetEntitySize())
        .WillByDefault(Return(EntitySize::Medium));
    ON_CALL(*mock, GetSpeedMeters()).WillByDefault(Return(9.0));
    return mock;
  }
};

TEST_F(MovementResolverTest, PlanMoveCardinalPath) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto entity = MakeMockEntity(1);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);

  MovementResolver resolver(&map);
  auto path = resolver.PlanMove(entity.get(), {3, 0}, 6);

  std::vector<GridPos> optimal_route =
      std::vector<GridPos>{{1, 0}, {2, 0}, {3, 0}};

  EXPECT_EQ(path, optimal_route);
}

TEST_F(MovementResolverTest, PlanMoveDiagonalPath) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto entity = MakeMockEntity(1);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);

  MovementResolver resolver(&map);
  auto path = resolver.PlanMove(entity.get(), {3, 3}, 6);

  std::vector<GridPos> optimal_route =
      std::vector<GridPos>{{1, 1}, {2, 2}, {3, 3}};

  EXPECT_EQ(path, optimal_route);
}

TEST_F(MovementResolverTest, PlanMove_PathOcuppied) {
  PositionMap map(20, 20, DiagonalMode::Alternating);
  auto entity = MakeMockEntity(1);
  auto obstacle = MakeMockEntity(2);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);
  map.Place(obstacle.get(), {2, 2}, EntitySize::Medium);

  MovementResolver resolver(&map);
  auto path = resolver.PlanMove(entity.get(), {3, 3}, 6);

  std::vector<GridPos> optimal_route =
      std::vector<GridPos>{{1, 1}, {2, 1}, {3, 2}, {3, 3}};

  EXPECT_EQ(path, optimal_route);
}

TEST_F(MovementResolverTest, PlanMove_PathOcuppied_Large) {
  PositionMap map(20, 20, DiagonalMode::Alternating);
  auto entity = MakeMockEntity(1);
  auto obstacle = MakeMockEntity(2);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);
  map.Place(obstacle.get(), {1, 1}, EntitySize::Large);

  MovementResolver resolver(&map);
  auto path = resolver.PlanMove(entity.get(), {3, 3}, 6);

  std::vector<GridPos> optimal_route =
      std::vector<GridPos>{{1, 0}, {2, 0}, {3, 1}, {3, 2}, {3, 3}};

  EXPECT_EQ(path, optimal_route);
}


TEST_F(MovementResolverTest, PlanMove_IsGreedy) {
  PositionMap map(20, 20, DiagonalMode::Alternating);
  auto entity = MakeMockEntity(1);
  auto large_obstacle = MakeMockEntity(2);
  auto medium_obstacle = MakeMockEntity(3);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);
  map.Place(large_obstacle.get(), {1, 1}, EntitySize::Large);
  map.Place(medium_obstacle.get(), {2, 0}, EntitySize::Medium);

  MovementResolver resolver(&map);
  auto path = resolver.PlanMove(entity.get(), {3, 3}, 6);

  std::vector<GridPos> greedy_route = std::vector<GridPos>{{1, 0}};

  EXPECT_EQ(path, greedy_route);
}

TEST_F(MovementResolverTest, PlanMoveRespectsBudget) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto entity = MakeMockEntity(1);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);

  MovementResolver resolver(&map);
  auto path = resolver.PlanMove(entity.get(), {10, 0}, 3);

  std::vector<GridPos> optimal_route =
      std::vector<GridPos>{{1, 0}, {2, 0}, {3, 0}};

  EXPECT_EQ(path, optimal_route);
}

TEST_F(MovementResolverTest, PlanMoveWithAlternatingDiagonals) {
  PositionMap map(20, 20, DiagonalMode::Alternating);
  auto entity = MakeMockEntity(1);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);

  MovementResolver resolver(&map);
  auto path = resolver.PlanMove(entity.get(), {4, 4}, 6);

  EXPECT_FALSE(path.empty());
  EXPECT_EQ(path.back(), (GridPos{4, 4}));
}

TEST_F(MovementResolverTest, PlanMoveWithFullDiagonals) {
  PositionMap map(20, 20, DiagonalMode::Full);
  auto entity = MakeMockEntity(1);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);

  MovementResolver resolver(&map);
  auto path = resolver.PlanMove(entity.get(), {3, 3}, 6);

  EXPECT_FALSE(path.empty());
  EXPECT_EQ(path.back(), (GridPos{3, 3}));
}

TEST_F(MovementResolverTest, ApplyMoveUpdatesPosition) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto entity = MakeMockEntity(1);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);

  MovementResolver resolver(&map);
  resolver.ApplyMove(entity.get(), {5, 5});

  EXPECT_EQ(map.GetAnchor(entity.get()), (GridPos{5, 5}));
}

TEST_F(MovementResolverTest, PlanMoveEmptyWhenAlreadyAtTarget) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto entity = MakeMockEntity(1);

  map.Place(entity.get(), {5, 5}, EntitySize::Medium);

  MovementResolver resolver(&map);
  auto path = resolver.PlanMove(entity.get(), {5, 5}, 6);

  EXPECT_TRUE(path.empty());
}

TEST_F(MovementResolverTest, FindClosestReachableCell) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto entity = MakeMockEntity(1);
  auto target = MakeMockEntity(2);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);
  map.Place(target.get(), {10, 0}, EntitySize::Medium);

  MovementResolver resolver(&map);
  GridPos closest = resolver.FindClosestReachableCell(
      entity.get(), target.get(), 6);

  EXPECT_EQ(closest, GridPos(6, 0));
}

TEST_F(MovementResolverTest, FindClosestReachableCell_IsOccupied) {
  PositionMap map(20, 20, DiagonalMode::Alternating);
  auto entity = MakeMockEntity(1);
  auto target = MakeMockEntity(2);
  auto obstacle = MakeMockEntity(3);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);
  map.Place(obstacle.get(), {6, 0}, EntitySize::Medium);
  map.Place(target.get(), {10, 0}, EntitySize::Medium);

  MovementResolver resolver(&map);
  GridPos closest = resolver.FindClosestReachableCell(
      entity.get(), target.get(), 6);

  EXPECT_EQ(closest.x, 6);
  EXPECT_NE(closest.y, 0);
}

}  // namespace positioning
}  // namespace ettes

// -----------------------------------------------------------------------------
// | @file      position_map_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "internal/positioning/position_map.h"
#include "tests/mocks/mock_entity.h"

using ::testing::Return;

namespace ettes {
namespace positioning {

class PositionMapTest : public ::testing::Test {
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

TEST_F(PositionMapTest, PlaceAndGetAnchor) {
  PositionMap map(20, 20);
  auto entity = MakeMockEntity(1);

  map.Place(entity.get(), {5, 3}, EntitySize::Medium);

  EXPECT_TRUE(map.HasEntity(entity.get()));
  EXPECT_EQ(map.GetAnchor(entity.get()), (GridPos{5, 3}));
  EXPECT_EQ(map.GetSize(entity.get()), EntitySize::Medium);
}

TEST_F(PositionMapTest, MoveEntity) {
  PositionMap map(20, 20);
  auto entity = MakeMockEntity(1);

  map.Place(entity.get(), {0, 0}, EntitySize::Medium);
  map.Move(entity.get(), {3, 4});

  EXPECT_EQ(map.GetAnchor(entity.get()), (GridPos{3, 4}));
}

TEST_F(PositionMapTest, RemoveEntity) {
  PositionMap map(20, 20);
  auto entity = MakeMockEntity(1);

  map.Place(entity.get(), {5, 5}, EntitySize::Medium);
  map.Remove(entity.get());

  EXPECT_FALSE(map.HasEntity(entity.get()));
}

TEST_F(PositionMapTest, DistanceBetweenMediumEntities) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto a = MakeMockEntity(1);
  auto b = MakeMockEntity(2);

  map.Place(a.get(), {0, 0}, EntitySize::Medium);
  map.Place(b.get(), {3, 0}, EntitySize::Medium);

  EXPECT_EQ(map.GetDistanceCells(a.get(), b.get()), 3);
  EXPECT_DOUBLE_EQ(map.GetDistanceMeters(a.get(), b.get()), 4.5);
}

TEST_F(PositionMapTest, DistanceBetweenLargeEntities) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto a = MakeMockEntity(1);
  auto b = MakeMockEntity(2);

  map.Place(a.get(), {0, 0}, EntitySize::Large);
  map.Place(b.get(), {4, 0}, EntitySize::Large);

  EXPECT_EQ(map.GetDistanceCells(a.get(), b.get()), 3);
}

TEST_F(PositionMapTest, AdjacentEntities) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto a = MakeMockEntity(1);
  auto b = MakeMockEntity(2);

  map.Place(a.get(), {0, 0}, EntitySize::Medium);
  map.Place(b.get(), {1, 0}, EntitySize::Medium);

  EXPECT_TRUE(map.AreAdjacent(a.get(), b.get()));
}

TEST_F(PositionMapTest, NotAdjacentEntities) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto a = MakeMockEntity(1);
  auto b = MakeMockEntity(2);

  map.Place(a.get(), {0, 0}, EntitySize::Medium);
  map.Place(b.get(), {3, 0}, EntitySize::Medium);

  EXPECT_FALSE(map.AreAdjacent(a.get(), b.get()));
}

TEST_F(PositionMapTest, GetAdjacentEntitiesReturnsList) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto a = MakeMockEntity(1);
  auto b = MakeMockEntity(2);
  auto c = MakeMockEntity(3);

  map.Place(a.get(), {5, 5}, EntitySize::Medium);
  map.Place(b.get(), {6, 5}, EntitySize::Medium);
  map.Place(c.get(), {10, 10}, EntitySize::Medium);

  auto adjacent = map.GetAdjacentEntities(a.get());
  EXPECT_EQ(adjacent.size(), 1u);
}

TEST_F(PositionMapTest, GetEntitiesInRange) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto a = MakeMockEntity(1);
  auto b = MakeMockEntity(2);
  auto c = MakeMockEntity(3);

  map.Place(a.get(), {0, 0}, EntitySize::Medium);
  map.Place(b.get(), {2, 0}, EntitySize::Medium);
  map.Place(c.get(), {10, 10}, EntitySize::Medium);

  auto in_range = map.GetEntitiesInRange(a.get(), 4.5);
  EXPECT_EQ(in_range.size(), 1u);
}

TEST_F(PositionMapTest, IsCellOccupied) {
  PositionMap map(20, 20);
  auto a = MakeMockEntity(1);

  map.Place(a.get(), {5, 5}, EntitySize::Large);

  EXPECT_TRUE(map.IsCellOccupied({5, 5}));
  EXPECT_TRUE(map.IsCellOccupied({6, 6}));
  EXPECT_FALSE(map.IsCellOccupied({7, 5}));
}

TEST_F(PositionMapTest, IsAreaFree) {
  PositionMap map(20, 20);
  auto a = MakeMockEntity(1);

  map.Place(a.get(), {5, 5}, EntitySize::Medium);

  EXPECT_FALSE(map.IsAreaFree({5, 5}, EntitySize::Medium));
  EXPECT_TRUE(map.IsAreaFree({6, 5}, EntitySize::Medium));
  EXPECT_TRUE(map.IsAreaFree({5, 5}, EntitySize::Medium, a.get()));
}

TEST_F(PositionMapTest, DiagonalModeAlternatingDistance) {
  PositionMap map(20, 20, DiagonalMode::Alternating);
  auto a = MakeMockEntity(1);
  auto b = MakeMockEntity(2);

  map.Place(a.get(), {0, 0}, EntitySize::Medium);
  map.Place(b.get(), {4, 4}, EntitySize::Medium);

  EXPECT_EQ(map.GetDistanceCells(a.get(), b.get()), 6);
}

TEST_F(PositionMapTest, DiagonalModeFullDistance) {
  PositionMap map(20, 20, DiagonalMode::Full);
  auto a = MakeMockEntity(1);
  auto b = MakeMockEntity(2);

  map.Place(a.get(), {0, 0}, EntitySize::Medium);
  map.Place(b.get(), {2, 2}, EntitySize::Medium);

  EXPECT_EQ(map.GetDistanceCells(a.get(), b.get()), 4);
}

TEST_F(PositionMapTest, OverlappingLargeEntitiesAreAdjacent) {
  PositionMap map(20, 20, DiagonalMode::Simple);
  auto a = MakeMockEntity(1);
  auto b = MakeMockEntity(2);

  map.Place(a.get(), {0, 0}, EntitySize::Large);
  map.Place(b.get(), {2, 0}, EntitySize::Large);

  EXPECT_TRUE(map.AreAdjacent(a.get(), b.get()));
}

}  // namespace positioning
}  // namespace ettes

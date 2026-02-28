// -----------------------------------------------------------------------------
// | @file      auto_layout_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "internal/positioning/auto_layout.h"
#include "tests/mocks/mock_entity.h"

using ::testing::Return;

namespace ettes {
namespace positioning {

class AutoLayoutTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockEntity> MakeMockEntity(uint32_t id,
                                              EntitySize size =
                                                  EntitySize::Medium) {
    auto mock = std::make_unique<MockEntity>();
    ON_CALL(*mock, GetId()).WillByDefault(Return(id));
    ON_CALL(*mock, IsAlive()).WillByDefault(Return(true));
    ON_CALL(*mock, GetEntitySize()).WillByDefault(Return(size));
    ON_CALL(*mock, GetSpeedMeters()).WillByDefault(Return(9.0));
    return mock;
  }
};

TEST_F(AutoLayoutTest, PlacesAllEntities) {
  auto a1 = MakeMockEntity(1);
  auto a2 = MakeMockEntity(2);
  auto b1 = MakeMockEntity(3);

  std::vector<entities::IEntity*> side_a = {a1.get(), a2.get()};
  std::vector<entities::IEntity*> side_b = {b1.get()};

  PositionMap map(20, 20);

  AutoLayoutSides(&map, side_a, side_b);

  EXPECT_TRUE(map.HasEntity(a1.get()));
  EXPECT_TRUE(map.HasEntity(a2.get()));
  EXPECT_TRUE(map.HasEntity(b1.get()));
}

TEST_F(AutoLayoutTest, SidesAreSeparated) {
  auto a1 = MakeMockEntity(1);
  auto b1 = MakeMockEntity(2);

  std::vector<entities::IEntity*> side_a = {a1.get()};
  std::vector<entities::IEntity*> side_b = {b1.get()};

  PositionMap map(20, 20);

  AutoLayoutSides(&map, side_a, side_b);

  GridPos pos_a = map.GetAnchor(a1.get());
  GridPos pos_b = map.GetAnchor(b1.get());

  EXPECT_LT(pos_a.y, pos_b.y);
}

TEST_F(AutoLayoutTest, RecommendedDimensionsArePositive) {
  auto a1 = MakeMockEntity(1);
  auto b1 = MakeMockEntity(2);

  std::vector<entities::IEntity*> side_a = {a1.get()};
  std::vector<entities::IEntity*> side_b = {b1.get()};

  EXPECT_GE(RecommendedGridWidth(side_a, side_b), 10);
  EXPECT_GE(RecommendedGridHeight(side_a, side_b), 10);
}

TEST_F(AutoLayoutTest, LargeEntitiesPlacedCorrectly) {
  auto a1 = MakeMockEntity(1, EntitySize::Large);
  auto b1 = MakeMockEntity(2, EntitySize::Large);

  std::vector<entities::IEntity*> side_a = {a1.get()};
  std::vector<entities::IEntity*> side_b = {b1.get()};

  int width = RecommendedGridWidth(side_a, side_b);
  int height = RecommendedGridHeight(side_a, side_b);
  PositionMap map(width, height);

  AutoLayoutSides(&map, side_a, side_b);

  EXPECT_TRUE(map.HasEntity(a1.get()));
  EXPECT_TRUE(map.HasEntity(b1.get()));
  EXPECT_EQ(map.GetSize(a1.get()), EntitySize::Large);
  EXPECT_EQ(map.GetSize(b1.get()), EntitySize::Large);
}

}  // namespace positioning
}  // namespace ettes

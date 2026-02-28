// -----------------------------------------------------------------------------
// | @file      encounter_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "internal/engine/encounter.h"
#include "tests/mocks/mock_entity.h"

using ::testing::Return;
using ::testing::ReturnRef;

namespace ettes {
namespace engine {

class EncounterTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockEntity> MakeMockEntity(uint32_t id, bool alive = true) {
    auto mock = std::make_unique<MockEntity>();
    ON_CALL(*mock, GetId()).WillByDefault(Return(id));
    ON_CALL(*mock, IsAlive()).WillByDefault(Return(alive));
    return mock;
  }
};

TEST_F(EncounterTest, IsOverWhenOneSideHasNoLivingEntities) {
  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(MakeMockEntity(1, true));

  std::vector<std::unique_ptr<entities::IEntity>> side_b;

  Encounter enc(std::move(side_a), std::move(side_b));

  EXPECT_TRUE(enc.IsOver());
}

TEST_F(EncounterTest, IsNotOverWhenBothSidesHaveLiving) {
  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(MakeMockEntity(1, true));

  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(MakeMockEntity(2, true));

  Encounter enc(std::move(side_a), std::move(side_b));

  EXPECT_FALSE(enc.IsOver());
}

TEST_F(EncounterTest, NotifyDeathMakesEncounterOver) {
  auto mock_a = MakeMockEntity(1, true);
  auto* raw_a = mock_a.get();

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(mock_a));

  auto mock_b = MakeMockEntity(2, true);

  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(mock_b));

  Encounter enc(std::move(side_a), std::move(side_b));

  EXPECT_FALSE(enc.IsOver());

  enc.NotifyDeath(raw_a);

  EXPECT_TRUE(enc.IsOver());
}

TEST_F(EncounterTest, GetLivingEnemiesOfReturnsOpposingSide) {
  auto mock_a = MakeMockEntity(1, true);
  auto* raw_a = mock_a.get();

  auto mock_b1 = MakeMockEntity(2, true);
  auto mock_b2 = MakeMockEntity(3, false);

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(mock_a));

  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(mock_b1));
  side_b.push_back(std::move(mock_b2));

  Encounter enc(std::move(side_a), std::move(side_b));

  auto enemies = enc.GetLivingEnemiesOf(raw_a);

  EXPECT_EQ(enemies.size(), 1u);
}

TEST_F(EncounterTest, GetFirstLivingEnemyReturnsFirstAlive) {
  auto mock_a = MakeMockEntity(1, true);
  auto* raw_a = mock_a.get();

  auto mock_b_dead = MakeMockEntity(2, false);
  auto mock_b_alive = MakeMockEntity(3, true);
  auto* raw_b_alive = mock_b_alive.get();

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(mock_a));

  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(mock_b_dead));
  side_b.push_back(std::move(mock_b_alive));

  Encounter enc(std::move(side_a), std::move(side_b));

  EXPECT_EQ(enc.GetFirstLivingEnemyOf(raw_a), raw_b_alive);
}

TEST_F(EncounterTest, GetFirstLivingEnemyReturnsNullWhenNoneAlive) {
  auto mock_a = MakeMockEntity(1, true);
  auto* raw_a = mock_a.get();

  auto mock_b = MakeMockEntity(2, false);

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(mock_a));

  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(std::move(mock_b));

  Encounter enc(std::move(side_a), std::move(side_b));

  EXPECT_EQ(enc.GetFirstLivingEnemyOf(raw_a), nullptr);
}

TEST_F(EncounterTest, GetLivingAlliesOfReturnsSameSide) {
  auto mock_a1 = MakeMockEntity(1, true);
  auto* raw_a1 = mock_a1.get();
  auto mock_a2 = MakeMockEntity(2, true);

  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(std::move(mock_a1));
  side_a.push_back(std::move(mock_a2));

  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(MakeMockEntity(3, true));

  Encounter enc(std::move(side_a), std::move(side_b));

  auto allies = enc.GetLivingAlliesOf(raw_a1);

  EXPECT_EQ(allies.size(), 2u);
}

TEST_F(EncounterTest, HasLivingEntitiesOnSidesReportsCorrectly) {
  std::vector<std::unique_ptr<entities::IEntity>> side_a;
  side_a.push_back(MakeMockEntity(1, true));

  std::vector<std::unique_ptr<entities::IEntity>> side_b;
  side_b.push_back(MakeMockEntity(2, true));

  Encounter enc(std::move(side_a), std::move(side_b));

  EXPECT_TRUE(enc.HasLivingEntitiesOnSideA());
  EXPECT_TRUE(enc.HasLivingEntitiesOnSideB());
}

}  // namespace engine
}  // namespace ettes

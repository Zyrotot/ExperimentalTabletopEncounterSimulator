// -----------------------------------------------------------------------------
// | @file      encounter.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/engine/encounter.h"

#include "internal/entities/i_entity.h"

namespace internal {
namespace engine {

Encounter::Encounter(std::vector<std::shared_ptr<entities::IEntity>> side_a,
                     std::vector<std::shared_ptr<entities::IEntity>> side_b)
    : side_a_(std::move(side_a)), side_b_(std::move(side_b)) {
}

int Encounter::FindSideOf(const entities::IEntity* entity) const {
  for (const auto& e : side_a_) {
    if (e.get() == entity) return 0;
  }
  for (const auto& e : side_b_) {
    if (e.get() == entity) return 1;
  }
  return -1;
}

std::vector<std::shared_ptr<entities::IEntity>> Encounter::GetLivingEnemiesOf(
    const entities::IEntity* entity) const {
  int side = FindSideOf(entity);
  const auto& enemies = (side == 0) ? side_b_ : side_a_;

  std::vector<std::shared_ptr<entities::IEntity>> living;
  for (const auto& e : enemies) {
    if (e && e->IsAlive()) {
      living.push_back(e);
    }
  }
  return living;
}

std::shared_ptr<entities::IEntity> Encounter::GetFirstLivingEnemyOf(
    const entities::IEntity* entity) const {
  auto living = GetLivingEnemiesOf(entity);
  return living.empty() ? nullptr : living.front();
}

std::vector<std::shared_ptr<entities::IEntity>> Encounter::GetLivingAlliesOf(
    const entities::IEntity* entity) const {
  int side = FindSideOf(entity);
  const auto& allies = (side == 0) ? side_a_ : side_b_;

  std::vector<std::shared_ptr<entities::IEntity>> living;
  for (const auto& e : allies) {
    if (e && e->IsAlive() && e.get() != entity) {
      living.push_back(e);
    }
  }
  return living;
}

bool Encounter::HasLivingEntitiesOnSideA() const {
  for (const auto& e : side_a_) {
    if (e && e->IsAlive()) return true;
  }
  return false;
}

bool Encounter::HasLivingEntitiesOnSideB() const {
  for (const auto& e : side_b_) {
    if (e && e->IsAlive()) return true;
  }
  return false;
}

bool Encounter::IsOver() const {
  return !HasLivingEntitiesOnSideA() || !HasLivingEntitiesOnSideB();
}

const std::vector<std::shared_ptr<entities::IEntity>>& Encounter::GetSideA()
    const {
  return side_a_;
}

const std::vector<std::shared_ptr<entities::IEntity>>& Encounter::GetSideB()
    const {
  return side_b_;
}

}  // namespace engine
}  // namespace internal

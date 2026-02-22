// -----------------------------------------------------------------------------
// | @file      encounter.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/engine/encounter.h"

#include "internal/entities/i_entity.h"

namespace ettes {
namespace engine {

Encounter::Encounter(std::vector<std::shared_ptr<entities::IEntity>> side_a,
                     std::vector<std::shared_ptr<entities::IEntity>> side_b)
    : side_a_(std::move(side_a)), side_b_(std::move(side_b)) {
  for (const auto& e : side_a_) {
    if (e) {
      side_map_[e->GetId()] = 0;
    }
  }
  for (const auto& e : side_b_) {
    if (e) {
      side_map_[e->GetId()] = 1;
    }
  }
}

int Encounter::FindSideOf(const entities::IEntity* entity) const {
  if (!entity) {
    return -1;
  }
  auto it = side_map_.find(entity->GetId());
  return (it != side_map_.end()) ? it->second : -1;
}

std::vector<std::shared_ptr<entities::IEntity>> Encounter::CollectLiving(
    int side,
    bool first_only) const {
  const auto& source = (side == 0) ? side_a_ : side_b_;
  std::vector<std::shared_ptr<entities::IEntity>> living;
  for (const auto& e : source) {
    if (e && e->IsAlive()) {
      living.push_back(e);
      if (first_only)
        return living;
    }
  }
  return living;
}

std::vector<std::shared_ptr<entities::IEntity>> Encounter::GetLivingEnemiesOf(
    const entities::IEntity* entity) const {
  int side = FindSideOf(entity);
  return CollectLiving(side == 0 ? 1 : 0);
}

std::shared_ptr<entities::IEntity> Encounter::GetFirstLivingEnemyOf(
    const entities::IEntity* entity) const {
  int side = FindSideOf(entity);
  auto living = CollectLiving(side == 0 ? 1 : 0, true);
  return living.empty() ? nullptr : living.front();
}

std::vector<std::shared_ptr<entities::IEntity>> Encounter::GetLivingAlliesOf(
    const entities::IEntity* entity) const {
  int side = FindSideOf(entity);
  return CollectLiving(side, entity);
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
}  // namespace ettes

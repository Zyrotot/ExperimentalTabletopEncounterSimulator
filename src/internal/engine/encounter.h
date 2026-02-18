// -----------------------------------------------------------------------------
// | @file      encounter.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENGINE_ENCOUNTER_H_
#define SRC_INTERNAL_ENGINE_ENCOUNTER_H_

#include <memory>
#include <vector>

namespace internal {
namespace entities {
class Entity;
}  // namespace entities

namespace engine {

class Encounter {
 public:
  Encounter(std::vector<std::shared_ptr<entities::Entity>> side_a,
            std::vector<std::shared_ptr<entities::Entity>> side_b);

  std::vector<std::shared_ptr<entities::Entity>> GetLivingEnemiesOf(
      const entities::Entity* entity) const;

  std::shared_ptr<entities::Entity> GetFirstLivingEnemyOf(
      const entities::Entity* entity) const;

  std::vector<std::shared_ptr<entities::Entity>> GetLivingAlliesOf(
      const entities::Entity* entity) const;

  bool HasLivingEntitiesOnSideA() const;
  bool HasLivingEntitiesOnSideB() const;

  bool IsOver() const;

  const std::vector<std::shared_ptr<entities::Entity>>& GetSideA() const;
  const std::vector<std::shared_ptr<entities::Entity>>& GetSideB() const;

 private:
  int FindSideOf(const entities::Entity* entity) const;

  std::vector<std::shared_ptr<entities::Entity>> side_a_;
  std::vector<std::shared_ptr<entities::Entity>> side_b_;
};

}  // namespace engine
}  // namespace internal

#endif  // SRC_INTERNAL_ENGINE_ENCOUNTER_H_

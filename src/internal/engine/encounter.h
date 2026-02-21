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
class IEntity;
}  // namespace entities

namespace engine {

class Encounter {
 public:
  Encounter(std::vector<std::shared_ptr<entities::IEntity>> side_a,
            std::vector<std::shared_ptr<entities::IEntity>> side_b);

  std::vector<std::shared_ptr<entities::IEntity>> GetLivingEnemiesOf(
      const entities::IEntity* entity) const;

  std::shared_ptr<entities::IEntity> GetFirstLivingEnemyOf(
      const entities::IEntity* entity) const;

  std::vector<std::shared_ptr<entities::IEntity>> GetLivingAlliesOf(
      const entities::IEntity* entity) const;

  bool HasLivingEntitiesOnSideA() const;
  bool HasLivingEntitiesOnSideB() const;

  bool IsOver() const;

  const std::vector<std::shared_ptr<entities::IEntity>>& GetSideA() const;
  const std::vector<std::shared_ptr<entities::IEntity>>& GetSideB() const;

 private:
  int FindSideOf(const entities::IEntity* entity) const;

  std::vector<std::shared_ptr<entities::IEntity>> side_a_;
  std::vector<std::shared_ptr<entities::IEntity>> side_b_;
};

}  // namespace engine
}  // namespace internal

#endif  // SRC_INTERNAL_ENGINE_ENCOUNTER_H_

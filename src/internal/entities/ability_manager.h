// -----------------------------------------------------------------------------
// | @file      ability_manager.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_ABILITY_MANAGER_H_
#define SRC_INTERNAL_ENTITIES_ABILITY_MANAGER_H_

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

#include "internal/abilities/ability.h"
#include "internal/combat/effect.h"

namespace ettes {
namespace entities {

class AbilityManager {
 public:
  explicit AbilityManager(std::vector<abilities::Ability> abilities);

  bool HasAbility(abilities::AbilityId id) const;
  int GetAbilityStack(abilities::AbilityId id) const;
  void IncrementAbilityStack(abilities::AbilityId id);
  void DecrementAbilityStack(abilities::AbilityId id);
  void SetAbilityStack(abilities::AbilityId id, int value);

  const std::vector<abilities::Ability>& GetAbilities() const;
  const std::vector<const combat::Effect*>& GetActiveEffects() const;
  const std::vector<const combat::Effect*>& GetEffectsForEvent(
      combat::CombatEvent event) const;
  void BuildActiveEffects();

 private:
  static constexpr std::size_t kNumEvents = 9;
  std::vector<abilities::Ability> abilities_;
  std::unordered_map<std::string, abilities::Ability*> ability_index_;
  std::unordered_map<abilities::AbilityId, abilities::Ability*> ability_id_index_;
  std::vector<const combat::Effect*> active_effects_;
  std::array<std::vector<const combat::Effect*>, kNumEvents> bucketed_effects_;
};

}  // namespace entities
}  // namespace ettes

#endif  // SRC_INTERNAL_ENTITIES_ABILITY_MANAGER_H_

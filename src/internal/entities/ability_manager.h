// -----------------------------------------------------------------------------
// | @file      ability_manager.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_ABILITY_MANAGER_H_
#define SRC_INTERNAL_ENTITIES_ABILITY_MANAGER_H_

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

  bool HasAbility(const std::string& ability_name) const;
  int GetAbilityStack(const std::string& ability_name) const;
  void IncrementAbilityStack(const std::string& ability_name);
  void DecrementAbilityStack(const std::string& ability_name);
  void SetAbilityStack(const std::string& ability_name, int value);

  const std::vector<abilities::Ability>& GetAbilities() const;
  const std::vector<const combat::Effect*>& GetActiveEffects() const;
  void BuildActiveEffects();

 private:
  std::vector<abilities::Ability> abilities_;
  std::unordered_map<std::string, abilities::Ability*> ability_index_;
  std::vector<const combat::Effect*> active_effects_;
};

}  // namespace entities
}  // namespace ettes

#endif  // SRC_INTERNAL_ENTITIES_ABILITY_MANAGER_H_

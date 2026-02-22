// -----------------------------------------------------------------------------
// | @file      ability_manager.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/entities/ability_manager.h"

namespace ettes {
namespace entities {

AbilityManager::AbilityManager(std::vector<abilities::Ability> abilities)
    : abilities_(std::move(abilities)) {
  for (auto& ability : abilities_) {
    ability_index_[ability.name] = &ability;
  }
}

bool AbilityManager::HasAbility(const std::string& ability_name) const {
  return ability_index_.count(ability_name) > 0;
}

int AbilityManager::GetAbilityStack(const std::string& ability_name) const {
  auto it = ability_index_.find(ability_name);
  if (it == ability_index_.end()) {
    return 0;
  }
  return it->second->stack_count;
}

void AbilityManager::IncrementAbilityStack(const std::string& ability_name) {
  auto it = ability_index_.find(ability_name);
  if (it != ability_index_.end()) {
    it->second->stack_count++;
  }
}

void AbilityManager::DecrementAbilityStack(const std::string& ability_name) {
  auto it = ability_index_.find(ability_name);
  if (it != ability_index_.end() && it->second->stack_count > 0) {
    it->second->stack_count--;
  }
}

void AbilityManager::SetAbilityStack(const std::string& ability_name,
                                     int value) {
  auto it = ability_index_.find(ability_name);
  if (it != ability_index_.end()) {
    it->second->stack_count = value;
  }
}

const std::vector<abilities::Ability>& AbilityManager::GetAbilities() const {
  return abilities_;
}

const std::vector<const combat::Effect*>& AbilityManager::GetActiveEffects()
    const {
  return active_effects_;
}

void AbilityManager::BuildActiveEffects() {
  active_effects_.clear();

  for (const auto& ability : abilities_) {
    if (!ability.is_active) {
      continue;
    }
    for (const auto& effect : ability.effects) {
      active_effects_.push_back(&effect);
    }
  }
}

}  // namespace entities
}  // namespace ettes

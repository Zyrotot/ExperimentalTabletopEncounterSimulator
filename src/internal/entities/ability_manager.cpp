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
    if (ability.id != abilities::AbilityId::None) {
      ability_id_index_[ability.id] = &ability;
    }
  }
}

bool AbilityManager::HasAbility(abilities::AbilityId id) const {
  return ability_id_index_.count(id) > 0;
}

int AbilityManager::GetAbilityStack(abilities::AbilityId id) const {
  auto it = ability_id_index_.find(id);
  if (it == ability_id_index_.end()) {
    return 0;
  }
  return it->second->stack_count;
}

void AbilityManager::IncrementAbilityStack(abilities::AbilityId id) {
  auto it = ability_id_index_.find(id);
  if (it != ability_id_index_.end()) {
    it->second->stack_count++;
  }
}

void AbilityManager::DecrementAbilityStack(abilities::AbilityId id) {
  auto it = ability_id_index_.find(id);
  if (it != ability_id_index_.end() && it->second->stack_count > 0) {
    it->second->stack_count--;
  }
}

void AbilityManager::SetAbilityStack(abilities::AbilityId id, int value) {
  auto it = ability_id_index_.find(id);
  if (it != ability_id_index_.end()) {
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

const std::vector<const combat::Effect*>& AbilityManager::GetEffectsForEvent(
    combat::CombatEvent event) const {
  std::size_t idx = static_cast<std::size_t>(event);
  if (idx < kNumEvents) {
    return bucketed_effects_[idx];
  }
  static const std::vector<const combat::Effect*> empty;
  return empty;
}

void AbilityManager::BuildActiveEffects() {
  active_effects_.clear();
  for (auto& bucket : bucketed_effects_) {
    bucket.clear();
  }

  for (const auto& ability : abilities_) {
    if (!ability.is_active) {
      continue;
    }
    for (const auto& effect : ability.effects) {
      active_effects_.push_back(&effect);
      std::size_t idx = static_cast<std::size_t>(effect.trigger);
      if (idx < kNumEvents) {
        bucketed_effects_[idx].push_back(&effect);
      }
    }
  }
}

}  // namespace entities
}  // namespace ettes

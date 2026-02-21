// -----------------------------------------------------------------------------
// | @file      entity.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/entities/entity.h"

#include <atomic>

#include "internal/items/weapon.h"
#include "internal/logging/log_manager.h"

namespace internal {
namespace entities {

std::atomic<uint32_t> Entity::next_id_{0};

using combat::AttackSequence;
using items::Weapon;

Entity::Entity(const EntityConfig& config)
    : id_(next_id_.fetch_add(1)),
      name_(config.name),
      starting_stats_(config.starting_stats),
      current_stats_(config.starting_stats),
      equipped_weapons_(config.equipped_weapons),
      attack_sequences_(config.attack_sequences),
      abilities_(config.abilities),
      alignment_(config.alignment),
      logger_(internal::logging::LogManager::GetLogger("entity")) {
  BuildActiveEffects();
  for (auto& ability : abilities_) {
    ability_index_[ability.name] = &ability;
  }
}

Entity::~Entity() {
}

uint32_t Entity::GetId() const {
  return id_;
}

const std::string& Entity::GetName() const {
  return name_;
}

const Stats& Entity::GetCurrentStats() const {
  return current_stats_;
}

const Stats& Entity::GetStartingStats() const {
  return starting_stats_;
}

const std::vector<AttackSequence>& Entity::GetAttackSequences() const {
  return attack_sequences_;
}

const AttackSequence& Entity::GetAttackSequence(int index) const {
  if (index < 0 || static_cast<size_t>(index) >= attack_sequences_.size()) {
    return attack_sequences_[0];
  }
  return attack_sequences_[static_cast<size_t>(index)];
}

const std::vector<std::shared_ptr<Weapon>>& Entity::GetEquippedWeapons() const {
  return equipped_weapons_;
}

const rules::Alignment& Entity::GetAlignment() const {
  return alignment_;
}

const std::vector<abilities::Ability>& Entity::GetAbilities() const {
  return abilities_;
}

bool Entity::HasAbility(const std::string& ability_name) const {
  return ability_index_.count(ability_name) > 0;
}

int Entity::GetAbilityStack(const std::string& ability_name) const {
  auto it = ability_index_.find(ability_name);
  if (it == ability_index_.end()) {
    return 0;
  }
  return it->second->stack_count;
}

void Entity::IncrementAbilityStack(const std::string& ability_name) {
  auto it = ability_index_.find(ability_name);
  if (it != ability_index_.end()) {
    it->second->stack_count++;
  }
}

void Entity::DecrementAbilityStack(const std::string& ability_name) {
  auto it = ability_index_.find(ability_name);
  if (it != ability_index_.end() && it->second->stack_count > 0) {
    it->second->stack_count--;
  }
}

void Entity::SetAbilityStack(const std::string& ability_name, int value) {
  auto it = ability_index_.find(ability_name);
  if (it != ability_index_.end()) {
    it->second->stack_count = value;
  }
}

int Entity::GetEffectiveAC() const {
  return current_stats_.base_stats.armour_class +
         current_stats_.bonus_stats.ac_bonus;
}

int Entity::GetFortification() const {
  return current_stats_.base_stats.fortification;
}

Resistances Entity::GetResistances() const {
  return current_stats_.base_stats.resistances +
         current_stats_.bonus_stats.bonus_resistances;
}

void Entity::TakeDamage(int damage) {
  if (damage <= 0) {
    return;
  }

  if (current_stats_.bonus_stats.temporary_hp > 0) {
    if (damage <= current_stats_.bonus_stats.temporary_hp) {
      current_stats_.bonus_stats.temporary_hp -= damage;
      return;
    }
    damage -= current_stats_.bonus_stats.temporary_hp;
    current_stats_.bonus_stats.temporary_hp = 0;
  }

  current_stats_.base_stats.hp -= damage;
  if (current_stats_.base_stats.hp < 0) {
    current_stats_.base_stats.hp = 0;
  }
}

void Entity::Heal(int amount) {
  if (amount <= 0) {
    return;
  }

  current_stats_.base_stats.hp += amount;

  if (current_stats_.base_stats.hp > starting_stats_.base_stats.hp) {
    current_stats_.base_stats.hp = starting_stats_.base_stats.hp;
  }
}

void Entity::AddTempHP(int amount) {
  if (amount <= 0) {
    return;
  }

  if (current_stats_.bonus_stats.temporary_hp < amount) {
    current_stats_.bonus_stats.temporary_hp = amount;
  }
}

const std::vector<const combat::Effect*>& Entity::GetActiveEffects() const {
  return active_effects_;
}

void Entity::BuildActiveEffects() {
  active_effects_.clear();

  for (const auto& ability : abilities_) {
    if (!ability.is_active)
      continue;

    for (const auto& effect : ability.effects) {
      active_effects_.push_back(&effect);
    }
  }

  for (const auto& weapon : equipped_weapons_) {
    if (!weapon)
      continue;

    for (const auto& enchantment : weapon->enchantments) {
      for (const auto& effect : enchantment.effects) {
        active_effects_.push_back(&effect);
      }
    }
  }
}

bool Entity::IsAlive() const {
  return current_stats_.base_stats.hp > 0;
}

void Entity::AddDR(const rules::DamageReduction& dr, bool is_bonus) {
  auto& dr_list =
      is_bonus
          ? current_stats_.bonus_stats.bonus_resistances.damage_reductions
          : current_stats_.base_stats.resistances.damage_reductions;

  for (auto& existing : dr_list) {
    if (existing.bypass_types == dr.bypass_types &&
        existing.bypass_modifiers == dr.bypass_modifiers) {
      existing.amount += dr.amount;
      return;
    }
  }

  dr_list.push_back(dr);
}

void Entity::RemoveDR(int amount, bool from_bonus) {
  logger_->debug("Removing {} DR from {}", amount,
                 from_bonus ? "bonus DR" : "base DR");
  auto& dr_list =
      from_bonus
          ? current_stats_.bonus_stats.bonus_resistances.damage_reductions
          : current_stats_.base_stats.resistances.damage_reductions;

  for (auto it = dr_list.rbegin(); it != dr_list.rend() && amount > 0;) {
    if (it->amount <= amount) {
      amount -= it->amount;
      it = std::vector<rules::DamageReduction>::reverse_iterator(
          dr_list.erase(std::next(it).base()));
    } else {
      it->amount -= amount;
      amount = 0;
      ++it;
    }
  }
}

void Entity::ClearAllDR(bool from_bonus) {
  if (from_bonus) {
    current_stats_.bonus_stats.bonus_resistances.damage_reductions.clear();
  } else {
    current_stats_.base_stats.resistances.damage_reductions.clear();
  }
}

}  // namespace entities
}  // namespace internal

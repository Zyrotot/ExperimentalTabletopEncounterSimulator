// -----------------------------------------------------------------------------
// | @file      entity.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/entities/entity.h"

#include <atomic>

#include "internal/logging/log_manager.h"

namespace ettes {
namespace entities {

std::atomic<uint32_t> Entity::next_id_{0};

Entity::Entity(const EntityConfig& config)
    : id_(next_id_.fetch_add(1)),
      name_(config.name),
      starting_stats_(config.starting_stats),
      current_stats_(config.starting_stats),
      alignment_(config.alignment),
      logger_(ettes::logging::LogManager::GetLogger("entity")),
      health_component_(&current_stats_.base_stats, &current_stats_.bonus_stats,
                        &starting_stats_.base_stats),
      combat_profile_(config.equipped_weapons, config.attack_sequences, logger_),
      ability_manager_(config.abilities),
      defense_profile_(&current_stats_.base_stats, &current_stats_.bonus_stats,
                       logger_) {
  ability_manager_.BuildActiveEffects();
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

const std::vector<combat::AttackSequence>& Entity::GetAttackSequences() const {
  return combat_profile_.GetAttackSequences();
}

const combat::AttackSequence& Entity::GetAttackSequence(int index) const {
  return combat_profile_.GetAttackSequence(index);
}

const std::vector<items::Weapon>& Entity::GetEquippedWeapons() const {
  return combat_profile_.GetEquippedWeapons();
}

const rules::Alignment& Entity::GetAlignment() const {
  return alignment_;
}

const std::vector<abilities::Ability>& Entity::GetAbilities() const {
  return ability_manager_.GetAbilities();
}

bool Entity::HasAbility(const std::string& ability_name) const {
  return ability_manager_.HasAbility(ability_name);
}

int Entity::GetAbilityStack(const std::string& ability_name) const {
  return ability_manager_.GetAbilityStack(ability_name);
}

void Entity::IncrementAbilityStack(const std::string& ability_name) {
  ability_manager_.IncrementAbilityStack(ability_name);
}

void Entity::DecrementAbilityStack(const std::string& ability_name) {
  ability_manager_.DecrementAbilityStack(ability_name);
}

void Entity::SetAbilityStack(const std::string& ability_name, int value) {
  ability_manager_.SetAbilityStack(ability_name, value);
}

int Entity::GetEffectiveAC() const {
  return defense_profile_.GetEffectiveAC();
}

int Entity::GetFortification() const {
  return defense_profile_.GetFortification();
}

Resistances Entity::GetResistances() const {
  return defense_profile_.GetResistances();
}

void Entity::TakeDamage(int damage) {
  health_component_.TakeDamage(damage);
}

void Entity::Heal(int amount) {
  health_component_.Heal(amount);
}

void Entity::AddTempHP(int amount) {
  health_component_.AddTempHP(amount);
}

const std::vector<const combat::Effect*>& Entity::GetActiveEffects() const {
  return ability_manager_.GetActiveEffects();
}

void Entity::BuildActiveEffects() {
  ability_manager_.BuildActiveEffects();
}

bool Entity::IsAlive() const {
  return health_component_.IsAlive();
}

void Entity::AddDR(const rules::DamageReduction& dr, bool is_bonus) {
  defense_profile_.AddDR(dr, is_bonus);
}

void Entity::RemoveDR(int amount, bool from_bonus) {
  defense_profile_.RemoveDR(amount, from_bonus);
}

void Entity::ClearAllDR(bool from_bonus) {
  defense_profile_.ClearAllDR(from_bonus);
}

}  // namespace entities
}  // namespace ettes

// -----------------------------------------------------------------------------
// | @file      entity.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/entities/entity.h"

#include <cstddef>

namespace internal {
namespace entities {

using combat::AttackSequence;
using items::Weapon;

Entity::Entity(const EntityConfig &config)
    : name_(config.name), starting_stats_(config.starting_stats),
      current_stats_(config.starting_stats),
      equipped_weapons_(config.equipped_weapons),
      attack_sequences_(config.attack_sequences), alignment_(config.alignment) {
}

Entity::~Entity() {}

const std::string &Entity::GetName() const { return name_; }

const Stats &Entity::GetCurrentStats() const { return current_stats_; }

const Stats &Entity::GetStartingStats() const { return starting_stats_; }

const std::vector<AttackSequence> &Entity::GetAttackSequences() const {
  return attack_sequences_;
}

const AttackSequence &Entity::GetAttackSequence(const int &index) const {
  if (index < 0 || static_cast<size_t>(index) >= attack_sequences_.size()) {
    return attack_sequences_[0];
  }
  return attack_sequences_[static_cast<size_t>(index)];
}

const std::vector<std::shared_ptr<Weapon>> &Entity::GetEquippedWeapons() const {
  return equipped_weapons_;
}

const rules::Alignment &Entity::GetAlignment() const { return alignment_; }

int Entity::GetEffectiveAC() const {
  return current_stats_.base_stats.armour_class +
         current_stats_.bonus_stats.ac_bonus;
}

int Entity::GetFortification() const {
  return current_stats_.base_stats.fortification;
}

Resistances Entity::GetResistancesCopy() const {
  return current_stats_.base_stats.resistances;
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

bool Entity::IsAlive() const { return current_stats_.base_stats.hp > 0; }

} // namespace entities
} // namespace internal

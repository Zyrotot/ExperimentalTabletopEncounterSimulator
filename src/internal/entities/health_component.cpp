// -----------------------------------------------------------------------------
// | @file      health_component.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/entities/health_component.h"

namespace ettes {
namespace entities {

HealthComponent::HealthComponent(BaseStats* current_base,
                                 BonusStats* current_bonus,
                                 const BaseStats* starting_base)
    : current_base_(current_base),
      current_bonus_(current_bonus),
      starting_base_(starting_base) {}

void HealthComponent::TakeDamage(int damage) {
  if (damage <= 0) {
    return;
  }

  if (current_bonus_->temporary_hp > 0) {
    if (damage <= current_bonus_->temporary_hp) {
      current_bonus_->temporary_hp -= damage;
      return;
    }
    damage -= current_bonus_->temporary_hp;
    current_bonus_->temporary_hp = 0;
  }

  current_base_->hp -= damage;
  if (current_base_->hp < 0) {
    current_base_->hp = 0;
  }
}

void HealthComponent::Heal(int amount) {
  if (amount <= 0) {
    return;
  }

  current_base_->hp += amount;

  if (current_base_->hp > starting_base_->hp) {
    current_base_->hp = starting_base_->hp;
  }
}

void HealthComponent::AddTempHP(int amount) {
  if (amount <= 0) {
    return;
  }

  if (current_bonus_->temporary_hp < amount) {
    current_bonus_->temporary_hp = amount;
  }
}

bool HealthComponent::IsAlive() const {
  return current_base_->hp > 0;
}

}  // namespace entities
}  // namespace ettes

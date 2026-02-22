// -----------------------------------------------------------------------------
// | @file      defense_profile.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/entities/defense_profile.h"

namespace ettes {
namespace entities {

DefenseProfile::DefenseProfile(BaseStats* current_base,
                               BonusStats* current_bonus,
                               logging::Logger* logger)
    : current_base_(current_base),
      current_bonus_(current_bonus),
      logger_(logger) {
}

int DefenseProfile::GetEffectiveAC() const {
  return current_base_->armour_class + current_bonus_->ac_bonus;
}

int DefenseProfile::GetFortification() const {
  return current_base_->fortification;
}

Resistances DefenseProfile::GetResistances() const {
  if (resistances_dirty_) {
    cached_resistances_ =
        current_base_->resistances + current_bonus_->bonus_resistances;
    resistances_dirty_ = false;
  }
  return cached_resistances_;
}

void DefenseProfile::AddDR(const rules::DamageReduction& dr, bool is_bonus) {
  resistances_dirty_ = true;
  auto& dr_list = is_bonus ? current_bonus_->bonus_resistances.damage_reductions
                           : current_base_->resistances.damage_reductions;

  for (auto& existing : dr_list) {
    if (existing.bypass_types == dr.bypass_types &&
        existing.bypass_modifiers == dr.bypass_modifiers) {
      existing.amount += dr.amount;
      return;
    }
  }

  dr_list.push_back(dr);
}

void DefenseProfile::RemoveDR(int amount, bool from_bonus) {
  resistances_dirty_ = true;
  logger_->debug("Removing {} DR from {}", amount,
                 from_bonus ? "bonus DR" : "base DR");
  auto& dr_list = from_bonus
                      ? current_bonus_->bonus_resistances.damage_reductions
                      : current_base_->resistances.damage_reductions;

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

void DefenseProfile::ClearAllDR(bool from_bonus) {
  resistances_dirty_ = true;
  if (from_bonus) {
    current_bonus_->bonus_resistances.damage_reductions.clear();
  } else {
    current_base_->resistances.damage_reductions.clear();
  }
}

}  // namespace entities
}  // namespace ettes

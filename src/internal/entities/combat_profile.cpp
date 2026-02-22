// -----------------------------------------------------------------------------
// | @file      combat_profile.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/entities/combat_profile.h"

#include <cassert>

namespace ettes {
namespace entities {

CombatProfile::CombatProfile(std::vector<items::Weapon> weapons,
                             std::vector<combat::AttackSequence> sequences,
                             logging::Logger* logger)
    : equipped_weapons_(std::move(weapons)),
      attack_sequences_(std::move(sequences)),
      logger_(logger) {
}

const combat::AttackSequence& CombatProfile::GetAttackSequence(
    int index) const {
  assert(!attack_sequences_.empty());

  if (index < 0 || static_cast<size_t>(index) >= attack_sequences_.size()) {
    logger_->warn(
        "GetAttackSequence called with invalid index {}; falling back to index "
        "0",
        index);
    return attack_sequences_[0];
  }

  return attack_sequences_[static_cast<size_t>(index)];
}

const std::vector<combat::AttackSequence>& CombatProfile::GetAttackSequences()
    const {
  return attack_sequences_;
}

const std::vector<items::Weapon>& CombatProfile::GetEquippedWeapons() const {
  return equipped_weapons_;
}

}  // namespace entities
}  // namespace ettes

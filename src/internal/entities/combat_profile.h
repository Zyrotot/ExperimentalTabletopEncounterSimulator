// -----------------------------------------------------------------------------
// | @file      combat_profile.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_COMBAT_PROFILE_H_
#define SRC_INTERNAL_ENTITIES_COMBAT_PROFILE_H_

#include <vector>

#include "internal/combat/attack.h"
#include "internal/items/weapon.h"
#include "internal/logging/logger.h"

namespace internal {
namespace entities {

class CombatProfile {
 public:
  CombatProfile(std::vector<items::Weapon> weapons,
                std::vector<combat::AttackSequence> sequences,
                logging::Logger* logger);

  const combat::AttackSequence& GetAttackSequence(int index) const;
  const std::vector<combat::AttackSequence>& GetAttackSequences() const;
  const std::vector<items::Weapon>& GetEquippedWeapons() const;

 private:
  std::vector<items::Weapon> equipped_weapons_;
  std::vector<combat::AttackSequence> attack_sequences_;
  logging::Logger* logger_;
};

}  // namespace entities
}  // namespace internal

#endif  // SRC_INTERNAL_ENTITIES_COMBAT_PROFILE_H_

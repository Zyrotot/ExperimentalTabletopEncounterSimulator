// -----------------------------------------------------------------------------
// | @file      defense_profile.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_DEFENSE_PROFILE_H_
#define SRC_INTERNAL_ENTITIES_DEFENSE_PROFILE_H_

#include "internal/entities/stats.h"
#include "internal/logging/logger.h"
#include "internal/rules/resistances.h"

namespace ettes {
namespace entities {

class DefenseProfile {
 public:
  DefenseProfile(BaseStats* current_base, BonusStats* current_bonus,
                 logging::Logger* logger);

  int GetEffectiveAC() const;
  int GetFortification() const;
  Resistances GetResistances() const;

  void AddDR(const rules::DamageReduction& dr, bool is_bonus);
  void RemoveDR(int amount, bool from_bonus);
  void ClearAllDR(bool from_bonus);

 private:
  BaseStats* current_base_;
  BonusStats* current_bonus_;
  logging::Logger* logger_;
  mutable Resistances cached_resistances_;
  mutable bool resistances_dirty_ = true;
};

}  // namespace entities
}  // namespace ettes

#endif  // SRC_INTERNAL_ENTITIES_DEFENSE_PROFILE_H_

// -----------------------------------------------------------------------------
// | @file      i_combatant.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_I_COMBATANT_H_
#define SRC_INTERNAL_ENTITIES_I_COMBATANT_H_

#include "internal/combat/attack.h"
#include "internal/entities/stats.h"
#include "internal/rules/alignment.h"

namespace ettes {
namespace entities {

class ICombatant {
 public:
  virtual ~ICombatant() = default;

  virtual const Stats& GetCurrentStats() const = 0;
  virtual const combat::AttackSequence& GetAttackSequence(int index) const = 0;
  virtual const rules::Alignment& GetAlignment() const = 0;
  virtual int GetEffectiveAC() const = 0;
  virtual int GetFortification() const = 0;
};

}  // namespace entities
}  // namespace ettes

#endif  // SRC_INTERNAL_ENTITIES_I_COMBATANT_H_

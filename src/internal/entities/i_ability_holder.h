// -----------------------------------------------------------------------------
// | @file      i_ability_holder.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_I_ABILITY_HOLDER_H_
#define SRC_INTERNAL_ENTITIES_I_ABILITY_HOLDER_H_

#include <vector>

#include "internal/abilities/ability_id.h"
#include "internal/combat/combat_event_types.h"
#include "internal/combat/effect.h"

namespace ettes {
namespace entities {

class IAbilityHolder {
 public:
  virtual ~IAbilityHolder() = default;

  virtual bool HasAbility(abilities::AbilityId id) const = 0;
  virtual int GetAbilityStack(abilities::AbilityId id) const = 0;
  virtual void IncrementAbilityStack(abilities::AbilityId id) = 0;
  virtual void DecrementAbilityStack(abilities::AbilityId id) = 0;
  virtual void SetAbilityStack(abilities::AbilityId id, int value) = 0;
  virtual const std::vector<const combat::Effect*>& GetActiveEffects()
      const = 0;
  virtual const std::vector<const combat::Effect*>& GetEffectsForEvent(
      combat::CombatEvent event) const = 0;
  virtual void BuildActiveEffects() = 0;
};

}  // namespace entities
}  // namespace ettes

#endif  // SRC_INTERNAL_ENTITIES_I_ABILITY_HOLDER_H_

// -----------------------------------------------------------------------------
// | @file      i_entity.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_I_ENTITY_H_
#define SRC_INTERNAL_ENTITIES_I_ENTITY_H_

#include <cstdint>
#include <string>

#include "internal/entities/i_ability_holder.h"
#include "internal/entities/i_combatant.h"
#include "internal/entities/i_damageable.h"

namespace ettes {
namespace entities {

class IEntity : public ICombatant, public IDamageable, public IAbilityHolder {
 public:
  ~IEntity() override = default;

  virtual uint32_t GetId() const = 0;
  virtual const std::string& GetName() const = 0;
};

}  // namespace entities
}  // namespace ettes

#endif  // SRC_INTERNAL_ENTITIES_I_ENTITY_H_

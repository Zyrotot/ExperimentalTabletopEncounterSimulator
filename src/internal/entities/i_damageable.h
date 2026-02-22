// -----------------------------------------------------------------------------
// | @file      i_damageable.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_I_DAMAGEABLE_H_
#define SRC_INTERNAL_ENTITIES_I_DAMAGEABLE_H_

#include "internal/entities/stats.h"
#include "internal/rules/resistances.h"

namespace ettes {
namespace entities {

class IDamageable {
 public:
  virtual ~IDamageable() = default;

  virtual void TakeDamage(int damage) = 0;
  virtual void Heal(int amount) = 0;
  virtual void AddTempHP(int amount) = 0;
  virtual bool IsAlive() const = 0;
  virtual Resistances GetResistances() const = 0;
  virtual void AddDR(const rules::DamageReduction& dr,
                     bool is_bonus = true) = 0;
  virtual void RemoveDR(int amount, bool from_bonus = true) = 0;
  virtual void ClearAllDR(bool from_bonus = true) = 0;
};

}  // namespace entities
}  // namespace ettes

#endif  // SRC_INTERNAL_ENTITIES_I_DAMAGEABLE_H_

// -----------------------------------------------------------------------------
// | @file      i_entity.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_I_ENTITY_H_
#define SRC_INTERNAL_ENTITIES_I_ENTITY_H_

#include <string>
#include <vector>

#include "internal/combat/attack.h"
#include "internal/combat/effect.h"
#include "internal/entities/stats.h"
#include "internal/rules/alignment.h"
#include "internal/rules/resistances.h"

namespace internal {
namespace entities {

class IEntity {
 public:
  virtual ~IEntity() = default;

  virtual const std::string& GetName() const = 0;
  virtual const Stats& GetCurrentStats() const = 0;

  virtual const combat::AttackSequence& GetAttackSequence(int index) const = 0;
  virtual const rules::Alignment& GetAlignment() const = 0;

  virtual int GetEffectiveAC() const = 0;
  virtual int GetFortification() const = 0;

  virtual Resistances GetResistances() const = 0;

  virtual void TakeDamage(int damage) = 0;
  virtual void Heal(int amount) = 0;
  virtual void AddTempHP(int amount) = 0;
  virtual bool IsAlive() const = 0;

  virtual bool HasAbility(const std::string& ability_name) const = 0;
  virtual int GetAbilityStack(const std::string& ability_name) const = 0;
  virtual void IncrementAbilityStack(const std::string& ability_name) = 0;
  virtual void DecrementAbilityStack(const std::string& ability_name) = 0;
  virtual void SetAbilityStack(const std::string& ability_name, int value) = 0;

  virtual void AddDR(const rules::DamageReduction& dr, bool is_bonus = true) = 0;
  virtual void RemoveDR(int amount, bool from_bonus = true) = 0;
  virtual void ClearAllDR(bool from_bonus = true) = 0;

  virtual const std::vector<combat::Effect>& GetActiveEffects() const = 0;
  virtual void BuildActiveEffects() = 0;
};

}  // namespace entities
}  // namespace internal

#endif  // SRC_INTERNAL_ENTITIES_I_ENTITY_H_

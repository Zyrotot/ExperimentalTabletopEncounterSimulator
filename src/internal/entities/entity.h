// -----------------------------------------------------------------------------
// | @file      entity.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_ENTITY_H_
#define SRC_INTERNAL_ENTITIES_ENTITY_H_

#include <memory>
#include <string>
#include <vector>

#include "internal/abilities/ability.h"
#include "internal/combat/attack.h"
#include "internal/combat/combat_events.h"
#include "internal/entities/stats.h"
#include "internal/logging/logger.h"
#include "internal/rules/alignment.h"

namespace internal {

namespace logging {
class Logger;
}  // namespace logging

namespace items {
struct Weapon;
}  // namespace items

namespace entities {

struct EntityConfig {
  std::string name;

  Stats starting_stats;

  std::vector<std::shared_ptr<items::Weapon>> equipped_weapons;
  std::vector<combat::AttackSequence> attack_sequences;
  std::vector<abilities::Ability> abilities;

  rules::Alignment alignment;
};

class Entity {
 public:
  explicit Entity(const EntityConfig& config);
  virtual ~Entity();

  const std::string& GetName() const;
  const Stats& GetCurrentStats() const;
  const Stats& GetStartingStats() const;
  const std::vector<combat::AttackSequence>& GetAttackSequences() const;
  const combat::AttackSequence& GetAttackSequence(const int& index) const;
  const std::vector<std::shared_ptr<items::Weapon>>& GetEquippedWeapons() const;
  const rules::Alignment& GetAlignment() const;

  const std::vector<abilities::Ability>& GetAbilities() const;
  bool HasAbility(const std::string& ability_name) const;
  int GetAbilityStack(const std::string& ability_name) const;

  const std::vector<combat::Effect>& GetActiveEffects() const;
  void BuildActiveEffects();

  void IncrementAbilityStack(const std::string& ability_name);
  void DecrementAbilityStack(const std::string& ability_name);
  void SetAbilityStack(const std::string& ability_name, int value);

  int GetEffectiveAC() const;
  int GetFortification() const;

  Resistances GetResistances() const;

  void TakeDamage(int damage);
  void Heal(int amount);
  void AddTempHP(int amount);
  bool IsAlive() const;

  void AddDR(const rules::DamageReduction& dr, bool is_bonus = true);
  void RemoveDR(int amount, bool from_bonus = true);
  void ClearAllDR(bool from_bonus = true);

 protected:
  std::string name_;

  Stats starting_stats_;
  Stats current_stats_;

  std::vector<std::shared_ptr<items::Weapon>> equipped_weapons_;
  std::vector<combat::AttackSequence> attack_sequences_;
  std::vector<abilities::Ability> abilities_;

  rules::Alignment alignment_;

  std::vector<combat::Effect> active_effects_;

  std::shared_ptr<logging::Logger> logger_;
};

}  // namespace entities
}  // namespace internal

#endif  // SRC_INTERNAL_ENTITIES_ENTITY_H_

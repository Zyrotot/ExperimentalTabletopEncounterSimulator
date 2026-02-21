// -----------------------------------------------------------------------------
// | @file      entity.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_ENTITY_H_
#define SRC_INTERNAL_ENTITIES_ENTITY_H_

#include <atomic>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "internal/abilities/ability.h"
#include "internal/combat/attack.h"
#include "internal/combat/effect.h"
#include "internal/entities/entity_config.h"
#include "internal/entities/i_entity.h"
#include "internal/entities/stats.h"
#include "internal/logging/logger.h"
#include "internal/rules/alignment.h"

namespace internal {

namespace entities {

class Entity : public IEntity {
 public:
  explicit Entity(const EntityConfig& config);
  ~Entity() override;

  uint32_t GetId() const override;
  const std::string& GetName() const override;
  const Stats& GetCurrentStats() const override;
  const Stats& GetStartingStats() const;
  const std::vector<combat::AttackSequence>& GetAttackSequences() const;
  const combat::AttackSequence& GetAttackSequence(int index) const override;
  const std::vector<items::Weapon>& GetEquippedWeapons() const;
  const rules::Alignment& GetAlignment() const override;

  const std::vector<abilities::Ability>& GetAbilities() const;
  bool HasAbility(const std::string& ability_name) const override;
  int GetAbilityStack(const std::string& ability_name) const override;

  const std::vector<const combat::Effect*>& GetActiveEffects() const override;
  void BuildActiveEffects() override;

  void IncrementAbilityStack(const std::string& ability_name) override;
  void DecrementAbilityStack(const std::string& ability_name) override;
  void SetAbilityStack(const std::string& ability_name, int value) override;

  int GetEffectiveAC() const override;
  int GetFortification() const override;

  Resistances GetResistances() const override;

  void TakeDamage(int damage) override;
  void Heal(int amount) override;
  void AddTempHP(int amount) override;
  bool IsAlive() const override;

  void AddDR(const rules::DamageReduction& dr, bool is_bonus = true) override;
  void RemoveDR(int amount, bool from_bonus = true) override;
  void ClearAllDR(bool from_bonus = true) override;

 protected:
  uint32_t id_;
  std::string name_;

  Stats starting_stats_;
  Stats current_stats_;

  std::vector<items::Weapon> equipped_weapons_;
  std::vector<combat::AttackSequence> attack_sequences_;
  std::vector<abilities::Ability> abilities_;

  rules::Alignment alignment_;

  std::vector<const combat::Effect*> active_effects_;
  std::unordered_map<std::string, abilities::Ability*> ability_index_;

  logging::Logger* logger_;

  static std::atomic<uint32_t> next_id_;
};

}  // namespace entities
}  // namespace internal

#endif  // SRC_INTERNAL_ENTITIES_ENTITY_H_

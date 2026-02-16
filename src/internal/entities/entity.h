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

#include "internal/combat/attack.h"
#include "internal/entities/stats.h"
#include "internal/items/weapon.h"
#include "internal/rules/alignment.h"

namespace internal {
namespace entities {

using combat::AttackSequence;
using items::Weapon;

struct EntityConfig {
  std::string name;

  Stats starting_stats;

  std::vector<std::shared_ptr<Weapon>> equipped_weapons;
  std::vector<AttackSequence> attack_sequences;

  rules::Alignment alignment;
};

class Entity {
 public:
  explicit Entity(const EntityConfig& config);
  virtual ~Entity();

  const std::string& GetName() const;
  const Stats& GetCurrentStats() const;
  const Stats& GetStartingStats() const;
  const std::vector<AttackSequence>& GetAttackSequences() const;
  const AttackSequence& GetAttackSequence(const int& index) const;
  const std::vector<std::shared_ptr<Weapon>>& GetEquippedWeapons() const;
  const rules::Alignment& GetAlignment() const;

  int GetEffectiveAC() const;

  void TakeDamage(int damage);
  void Heal(int amount);
  bool IsAlive() const;

 protected:
  std::string name_;

  Stats starting_stats_;
  Stats current_stats_;

  std::vector<std::shared_ptr<Weapon>> equipped_weapons_;
  std::vector<AttackSequence> attack_sequences_;

  rules::Alignment alignment_;
};

}  // namespace entities
}  // namespace internal

#endif  // SRC_INTERNAL_ENTITIES_ENTITY_H_

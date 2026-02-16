// -----------------------------------------------------------------------------
// | @file      attack_resolver.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RESOLVER_ATTACK_RESOLVER_H_
#define SRC_INTERNAL_RESOLVER_ATTACK_RESOLVER_H_

#include <memory>
#include <vector>

#include "internal/combat/attack.h"
#include "internal/combat/combat_events.h"
#include "internal/items/enchantment.h"
#include "internal/rules/damage_types.h"

namespace internal {

namespace entities {
class Entity;
}  // namespace entities

namespace dice_rolls {
class Roller;
}  // namespace dice_rolls

namespace logging {
class Logger;
}  // namespace logging

namespace resolver {

using combat::AttackMove;
using combat::AttackSequence;
using combat::CombatContext;
using combat::CombatEvent;
using dice_rolls::Roller;
using dice_rolls::Term;
using entities::Entity;
using logging::Logger;
using rules::DamageInstance;
using rules::DamageModifier;
using rules::DamageType;

class AttackResolver {
 public:
  AttackResolver(std::shared_ptr<Entity> attacker,
                 std::shared_ptr<Entity> defender,
                 const int& attack_sequence_index,
                 std::shared_ptr<Roller> roller);
  ~AttackResolver();

  std::vector<CombatContext> ResolveAttack();

 protected:
  CombatContext ResolveAttackMove(const AttackMove& attack_move);

  DamageInstance CalculateBaseDamage(const AttackMove& attack_move,
                                     int crit_multiplier);
  void GatherDamageFromSources(const AttackMove& attack_move,
                               CombatContext* context);

  void EmitEvent(CombatEvent event, CombatContext* context);

  Term CalculateTotalDamage(const AttackMove& attack_move);
  int CalculateTotalAttackModifier(const AttackMove& attack_move);
  int CheckCriticalHit(const AttackMove& attack_move, const int& attack_roll);

 private:
  std::shared_ptr<Entity> attacker_;
  std::shared_ptr<Entity> defender_;
  AttackSequence attack_sequence_;
  std::shared_ptr<Roller> roller_;
  std::shared_ptr<Logger> logger_;
};

}  // namespace resolver
}  // namespace internal

#endif  // SRC_INTERNAL_RESOLVER_ATTACK_RESOLVER_H_

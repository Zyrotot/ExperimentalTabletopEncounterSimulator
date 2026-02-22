// -----------------------------------------------------------------------------
// | @file      attack_resolver.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RESOLVER_ATTACK_RESOLVER_H_
#define SRC_INTERNAL_RESOLVER_ATTACK_RESOLVER_H_

#include "internal/combat/attack.h"
#include "internal/combat/attack_result.h"
#include "internal/combat/combat_context.h"
#include "internal/items/enchantment.h"
#include "internal/logging/logger.h"
#include "internal/rules/damage_types.h"

namespace ettes {

namespace resolver {

class AttackResolver {
 public:
  AttackResolver(const combat::AttackSequence& attack_sequence,
                 combat::CombatEventContext* context);
  ~AttackResolver();

  void ResolveSingleAttack(size_t move_index);

 protected:
  void ResolveAttackMove(const combat::AttackMove& attack_move);

  rules::DamageInstance CalculateBaseDamage(
      const combat::AttackMove& attack_move, int crit_multiplier);
  void GatherDamageFromSources(const combat::AttackMove& attack_move,
                               combat::AttackResult* result);

  dice_rolls::Term CalculateTotalDamage(const combat::AttackMove& attack_move);
  int CalculateTotalAttackModifier(const combat::AttackMove& attack_move);
  int CheckCriticalHit(const combat::AttackMove& attack_move,
                       const int& attack_roll, const int& fortification);

 private:
  combat::AttackSequence attack_sequence_;
  logging::Logger* logger_;
  combat::CombatEventContext* context_;
};

}  // namespace resolver
}  // namespace ettes

#endif  // SRC_INTERNAL_RESOLVER_ATTACK_RESOLVER_H_

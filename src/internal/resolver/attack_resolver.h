// -----------------------------------------------------------------------------
// | @file      attack_resolver.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RESOLVER_ATTACK_RESOLVER_H_
#define SRC_INTERNAL_RESOLVER_ATTACK_RESOLVER_H_

#include <memory>

#include "internal/combat/attack.h"
#include "internal/combat/attack_result.h"
#include "internal/combat/combat_context.h"
#include "internal/items/enchantment.h"
#include "internal/logging/logger.h"
#include "internal/rules/damage_types.h"

namespace internal {

namespace entities {
class IEntity;
}  // namespace entities

namespace dice_rolls {
class Roller;
}  // namespace dice_rolls

namespace resolver {

class AttackResolver {
 public:
  AttackResolver(std::shared_ptr<entities::IEntity> attacker,
                 std::shared_ptr<entities::IEntity> defender,
                 const int& attack_sequence_index,
                 std::shared_ptr<dice_rolls::Roller> roller);
  ~AttackResolver();

  std::shared_ptr<combat::CombatEventContext> ResolveAttack();

  void ResolveSingleAttack(size_t move_index,
                           std::shared_ptr<combat::CombatEventContext> context);

 protected:
  void ResolveAttackMove(const combat::AttackMove& attack_move,
                         std::shared_ptr<combat::CombatEventContext> context);

  rules::DamageInstance CalculateBaseDamage(
      const combat::AttackMove& attack_move, int crit_multiplier);
  void GatherDamageFromSources(
      const combat::AttackMove& attack_move,
      std::shared_ptr<combat::CombatEventContext> context,
      combat::AttackResult* result);

  dice_rolls::Term CalculateTotalDamage(const combat::AttackMove& attack_move);
  int CalculateTotalAttackModifier(const combat::AttackMove& attack_move);
  int CheckCriticalHit(const combat::AttackMove& attack_move,
                       const int& attack_roll, const int& fortification);

 private:
  std::shared_ptr<entities::IEntity> attacker_;
  std::shared_ptr<entities::IEntity> defender_;
  combat::AttackSequence attack_sequence_;
  std::shared_ptr<dice_rolls::Roller> roller_;
  std::shared_ptr<logging::Logger> logger_;
};

}  // namespace resolver
}  // namespace internal

#endif  // SRC_INTERNAL_RESOLVER_ATTACK_RESOLVER_H_

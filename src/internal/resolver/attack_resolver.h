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
#include "internal/dice_rolls/roller.h"
#include "internal/entities/entity.h"
#include "internal/logging/logger.h"

namespace internal {
namespace resolver {

using combat::AttackMove;
using combat::AttackSequence;
using dice_rolls::Roller;
using dice_rolls::Term;
using entities::Entity;
using logging::Logger;


class AttackResolver {
 public:
  struct AttackResult {
    bool hit = false;
    int d20_roll = 0;
    int total_roll = 0;
    int damage = 0;
    bool is_critical = false;
  };

  AttackResolver(std::shared_ptr<Entity> attacker,
                 std::shared_ptr<Entity> defender,
                 const int& attack_sequence_index,
                 std::shared_ptr<Roller> roller);
  ~AttackResolver();

  std::vector<AttackResult> ResolveAttack();

 protected:
  AttackResult ResolveAttackMove(const AttackMove& attack_move);

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

// -----------------------------------------------------------------------------
// | @file      attack_result.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_ATTACK_RESULT_H_
#define SRC_INTERNAL_COMBAT_ATTACK_RESULT_H_

#include <memory>
#include <vector>

#include "internal/rules/damage_types.h"

namespace ettes {

namespace entities {
class IEntity;
}

namespace combat {
struct AttackMove;
}

namespace combat {

struct AttackResult {
  const combat::AttackMove* attack;
  int d20_roll = 0;
  int total_attack_roll = 0;
  bool is_hit = false;
  bool is_crit = false;
  int crit_multiplier = 1;
  std::vector<rules::DamageInstance> damage_instances;
};

struct QueuedAttack {
  std::shared_ptr<entities::IEntity> attacker;
  std::shared_ptr<entities::IEntity> target;
  int attack_sequence_index = 0;
};

}  // namespace combat
}  // namespace ettes

#endif  // SRC_INTERNAL_COMBAT_ATTACK_RESULT_H_

// -----------------------------------------------------------------------------
// | @file      combat_context.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_COMBAT_CONTEXT_H_
#define SRC_INTERNAL_COMBAT_COMBAT_CONTEXT_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "internal/combat/attack_queue.h"
#include "internal/combat/attack_result.h"

namespace internal {

namespace entities {
class IEntity;
}

namespace dice_rolls {
class Roller;
}

namespace combat {

struct CombatEventContext {
  std::shared_ptr<entities::IEntity> source;
  std::shared_ptr<entities::IEntity> target;

  dice_rolls::Roller* roller = nullptr;
  std::vector<AttackResult> results;
  size_t current_index = 0;

  int heal_amount = 0;
  bool is_temp_hp = false;

  IAttackQueue* attack_queue = nullptr;
};

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_COMBAT_CONTEXT_H_

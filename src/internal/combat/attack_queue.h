// -----------------------------------------------------------------------------
// | @file      attack_queue.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_ATTACK_QUEUE_H_
#define SRC_INTERNAL_COMBAT_ATTACK_QUEUE_H_

#include "internal/combat/attack_result.h"

namespace ettes {
namespace combat {

class IAttackQueue {
 public:
  virtual ~IAttackQueue() = default;
  virtual void QueueAttack(QueuedAttack attack) = 0;
};

}  // namespace combat
}  // namespace ettes

#endif  // SRC_INTERNAL_COMBAT_ATTACK_QUEUE_H_

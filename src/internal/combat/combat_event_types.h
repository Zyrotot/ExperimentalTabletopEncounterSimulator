// -----------------------------------------------------------------------------
// | @file      combat_event_types.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_COMBAT_EVENT_TYPES_H_
#define SRC_INTERNAL_COMBAT_COMBAT_EVENT_TYPES_H_

namespace internal {
namespace combat {

enum class CombatEvent {
  TurnStart,
  AttackRoll,
  Hit,
  Miss,
  DealDamage,
  TakeDamage,
  Kill,
  Heal,
  CriticalHit,
};

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_COMBAT_EVENT_TYPES_H_

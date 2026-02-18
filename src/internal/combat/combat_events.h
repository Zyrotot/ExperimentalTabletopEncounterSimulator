// -----------------------------------------------------------------------------
// | @file      combat_events.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_COMBAT_EVENTS_H_
#define SRC_INTERNAL_COMBAT_COMBAT_EVENTS_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "internal/rules/damage_types.h"

namespace internal {

namespace entities {
class Entity;
}

namespace combat {
struct AttackMove;
}

namespace dice_rolls {
class Roller;
}

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

struct AttackResult {
  const combat::AttackMove* attack;
  int d20_roll = 0;
  int total_attack_roll = 0;
  bool is_hit = false;
  bool is_crit = false;
  int crit_multiplier = 1;
  std::vector<rules::DamageInstance> damage_instances;
};

struct CombatEventContext {
  std::shared_ptr<entities::Entity> source;
  std::shared_ptr<entities::Entity> target;

  std::shared_ptr<dice_rolls::Roller> roller;
  std::vector<AttackResult> results;
  size_t current_index = 0;

  int heal_amount = 0;
  bool is_temp_hp = false;
};

struct Effect {
  std::string name;
  std::string source;
  CombatEvent trigger;
  std::function<void(std::shared_ptr<CombatEventContext>)> on_event;
  bool is_active = true;
};

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_COMBAT_EVENTS_H_

// -----------------------------------------------------------------------------
// | @file      combat_events.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_COMBAT_COMBAT_EVENTS_H_
#define SRC_INTERNAL_COMBAT_COMBAT_EVENTS_H_

#include <memory>
#include <vector>

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

namespace rules {
struct DamageInstance;
}

namespace combat {

enum class CombatEvent {
  TurnStart,   // Beginning of turn
  AttackRoll,  // After d20 is rolled, before hit/miss determination
  Hit,         // Attack hit (before damage)
  Miss,        // Attack missed
  DealDamage,  // After damage is calculated, before applying to target
  TakeDamage,  // Target receives damage
  Kill,        // Target HP drops to 0
  Heal,        // Entity receives healing
  CriticalHit  // Critical hit confirmed
};

struct CombatContext {
  std::shared_ptr<entities::Entity> attacker;
  std::shared_ptr<entities::Entity> target;
  const combat::AttackMove* attack;
  std::shared_ptr<dice_rolls::Roller> roller;

  // Attack roll results
  int d20_roll = 0;
  int total_attack_roll = 0;
  bool is_hit = false;
  bool is_crit = false;
  int crit_multiplier = 1;

  // Damage results (accumulated during resolution)
  std::vector<rules::DamageInstance> damage_instances;
};

struct DamageContext {
  std::shared_ptr<entities::Entity> attacker;
  std::shared_ptr<entities::Entity> target;
  std::vector<rules::DamageInstance> damage;
};

struct HealContext {
  std::shared_ptr<entities::Entity> source;
  std::shared_ptr<entities::Entity> target;
  int amount;
  bool is_temp_hp;
};

struct HitContext {
  std::shared_ptr<entities::Entity> attacker;
  std::shared_ptr<entities::Entity> target;
  bool is_crit;
};

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_COMBAT_EVENTS_H_

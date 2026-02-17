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

struct EventContext {  // TODO(zyrotot): Simplify context, this has gotten too
                       // complex
  virtual ~EventContext() = default;
  virtual std::vector<std::shared_ptr<entities::Entity>> GetInvolvedEntities()
      const = 0;
};

struct Effect {
  std::string name;
  std::string source;
  CombatEvent trigger;
  std::function<void(std::shared_ptr<EventContext>)> on_event;
  bool is_active = true;
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

struct CombatContext : EventContext {
  std::shared_ptr<entities::Entity> attacker;
  std::shared_ptr<entities::Entity> target;
  std::shared_ptr<dice_rolls::Roller> roller;
  std::vector<AttackResult> results;

  CombatContext() = default;
  CombatContext(std::shared_ptr<entities::Entity> attacker_,
                std::shared_ptr<entities::Entity> target_,
                std::shared_ptr<dice_rolls::Roller> roller_)
      : attacker(attacker_), target(target_), roller(roller_) {
  }

  std::vector<std::shared_ptr<entities::Entity>> GetInvolvedEntities()
      const override {
    std::vector<std::shared_ptr<entities::Entity>> entities;
    if (attacker)
      entities.push_back(attacker);
    if (target && target != attacker)
      entities.push_back(target);
    return entities;
  }
};

struct DamageContext : EventContext {
  std::shared_ptr<entities::Entity> attacker;
  std::shared_ptr<entities::Entity> target;
  std::vector<rules::DamageInstance> damage;

  DamageContext() = default;
  DamageContext(std::shared_ptr<entities::Entity> attacker_,
                std::shared_ptr<entities::Entity> target_)
      : attacker(attacker_), target(target_) {
  }

  std::vector<std::shared_ptr<entities::Entity>> GetInvolvedEntities()
      const override {
    std::vector<std::shared_ptr<entities::Entity>> entities;
    if (attacker)
      entities.push_back(attacker);
    if (target && target != attacker)
      entities.push_back(target);
    return entities;
  }
};

struct HealContext : EventContext {
  std::shared_ptr<entities::Entity> source;
  std::shared_ptr<entities::Entity> target;
  int amount = 0;
  bool is_temp_hp = false;

  HealContext() = default;

  std::vector<std::shared_ptr<entities::Entity>> GetInvolvedEntities()
      const override {
    std::vector<std::shared_ptr<entities::Entity>> entities;
    if (source)
      entities.push_back(source);
    if (target && target != source)
      entities.push_back(target);
    return entities;
  }
};

struct HitContext : EventContext {
  std::shared_ptr<entities::Entity> attacker;
  std::shared_ptr<entities::Entity> target;
  bool is_crit = false;

  HitContext() = default;
  HitContext(std::shared_ptr<entities::Entity> attacker_,
             std::shared_ptr<entities::Entity> target_, bool is_crit_ = false)
      : attacker(attacker_), target(target_), is_crit(is_crit_) {
  }

  std::vector<std::shared_ptr<entities::Entity>> GetInvolvedEntities()
      const override {
    std::vector<std::shared_ptr<entities::Entity>> entities;
    if (attacker)
      entities.push_back(attacker);
    if (target && target != attacker)
      entities.push_back(target);
    return entities;
  }
};

}  // namespace combat
}  // namespace internal

#endif  // SRC_INTERNAL_COMBAT_COMBAT_EVENTS_H_

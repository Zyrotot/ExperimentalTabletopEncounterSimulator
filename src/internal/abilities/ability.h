// -----------------------------------------------------------------------------
// | @file      ability.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ABILITIES_ABILITY_H_
#define SRC_INTERNAL_ABILITIES_ABILITY_H_

#include <functional>
#include <string>
#include <vector>

#include "internal/combat/combat_events.h"

namespace internal {

namespace entities {
class Entity;
}  // namespace entities

namespace abilities {

struct AbilityEffect {
  combat::CombatEvent trigger;
  std::function<void(std::shared_ptr<combat::EventContext>)> on_event;
};

struct Ability {
  std::string name;
  std::vector<AbilityEffect> effects;
  
  int stack_count = 0;
  bool is_active = true;
};

Ability CreateErosao();
Ability CreateRigidezRaivosa();
Ability CreateTrespassar();
Ability CreateDuroDeFerir(int stacks);
Ability CreateDuroDeMatar(int stacks);

}  // namespace abilities
}  // namespace internal

#endif  // SRC_INTERNAL_ABILITIES_ABILITY_H_

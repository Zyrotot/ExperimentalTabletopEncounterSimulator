// -----------------------------------------------------------------------------
// | @file      ability.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ABILITIES_ABILITY_H_
#define SRC_INTERNAL_ABILITIES_ABILITY_H_

#include <string>
#include <vector>

#include "internal/combat/combat_events.h"

namespace internal {

namespace entities {
class Entity;
}  // namespace entities

namespace abilities {

struct Ability {
  std::string name;
  std::vector<combat::Effect> effects;

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

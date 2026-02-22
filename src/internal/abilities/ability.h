// -----------------------------------------------------------------------------
// | @file      ability.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ABILITIES_ABILITY_H_
#define SRC_INTERNAL_ABILITIES_ABILITY_H_

#include <string>
#include <vector>

#include "internal/abilities/ability_id.h"
#include "internal/combat/effect.h"

namespace ettes {

namespace entities {
class Entity;
}  // namespace entities

namespace abilities {

struct Ability {
  std::string name;
  AbilityId id = AbilityId::None;
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
}  // namespace ettes

#endif  // SRC_INTERNAL_ABILITIES_ABILITY_H_

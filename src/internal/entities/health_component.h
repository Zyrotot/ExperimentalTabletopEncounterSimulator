// -----------------------------------------------------------------------------
// | @file      health_component.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_HEALTH_COMPONENT_H_
#define SRC_INTERNAL_ENTITIES_HEALTH_COMPONENT_H_

#include "internal/entities/stats.h"

namespace ettes {
namespace entities {

class HealthComponent {
 public:
  HealthComponent(BaseStats* current_base, BonusStats* current_bonus,
                  const BaseStats* starting_base);

  void TakeDamage(int damage);
  void Heal(int amount);
  void AddTempHP(int amount);
  bool IsAlive() const;

 private:
  BaseStats* current_base_;
  BonusStats* current_bonus_;
  const BaseStats* starting_base_;
};

}  // namespace entities
}  // namespace ettes

#endif  // SRC_INTERNAL_ENTITIES_HEALTH_COMPONENT_H_

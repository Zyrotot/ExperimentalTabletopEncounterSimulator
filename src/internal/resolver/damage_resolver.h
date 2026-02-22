// -----------------------------------------------------------------------------
// | @file      damage_resolver.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RESOLVER_DAMAGE_RESOLVER_H_
#define SRC_INTERNAL_RESOLVER_DAMAGE_RESOLVER_H_

#include "internal/combat/combat_context.h"
#include "internal/entities/stats.h"
#include "internal/logging/logger.h"

namespace ettes {

namespace resolver {

class DamageResolver {
 public:
  explicit DamageResolver(combat::CombatEventContext* context);
  ~DamageResolver();

  void ApplySingleAttack(size_t result_index);

  static void ApplyResistancesToDamage(rules::DamageInstance* dmg_instance,
                                       entities::Resistances* resistances);

 private:
  combat::CombatEventContext* context_;
  logging::Logger* logger_;
};

}  // namespace resolver
}  // namespace ettes

#endif  // SRC_INTERNAL_RESOLVER_DAMAGE_RESOLVER_H_

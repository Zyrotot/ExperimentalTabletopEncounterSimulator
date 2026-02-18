// -----------------------------------------------------------------------------
// | @file      damage_resolver.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RESOLVER_DAMAGE_RESOLVER_H_
#define SRC_INTERNAL_RESOLVER_DAMAGE_RESOLVER_H_

#include <memory>

#include "internal/combat/combat_events.h"
#include "internal/entities/stats.h"

namespace internal {
namespace logging {
class Logger;
}  // namespace logging

namespace resolver {

class DamageResolver {
 public:
  explicit DamageResolver(std::shared_ptr<combat::CombatEventContext> context);
  ~DamageResolver();

  void ApplyDamage();

  void ApplySingleAttack(size_t result_index);

  static void ApplyResistancesToDamage(rules::DamageInstance* dmg_instance,
                                       entities::Resistances* resistances);

 private:
  std::shared_ptr<combat::CombatEventContext> context_;
  std::shared_ptr<logging::Logger> logger_;
};

}  // namespace resolver
}  // namespace internal

#endif  // SRC_INTERNAL_RESOLVER_DAMAGE_RESOLVER_H_

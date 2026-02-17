// -----------------------------------------------------------------------------
// | @file      damage_resolver.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RESOLVER_DAMAGE_RESOLVER_H_
#define SRC_INTERNAL_RESOLVER_DAMAGE_RESOLVER_H_

#include "internal/combat/combat_events.h"
#include "internal/entities/stats.h"

#include <memory>

namespace internal {
namespace resolver {

class DamageResolver {
public:
  DamageResolver(std::shared_ptr<combat::CombatContext> context);
  ~DamageResolver();

  void ApplyDamage();

protected:
  void ApplyResistances(rules::DamageInstance *dmg_instance,
                        entities::Resistances *resistances);

private:
  std::shared_ptr<combat::CombatContext> context_;
};

} // namespace resolver
} // namespace internal

#endif // SRC_INTERNAL_RESOLVER_DAMAGE_RESOLVER_H_

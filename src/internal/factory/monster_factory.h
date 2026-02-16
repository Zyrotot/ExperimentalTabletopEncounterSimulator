// -----------------------------------------------------------------------------
// | @file      monster_factory.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_FACTORY_MONSTER_FACTORY_H_
#define SRC_INTERNAL_FACTORY_MONSTER_FACTORY_H_

#include "internal/entities/entity.h"

namespace internal {
namespace factory {

using entities::Entity;

enum Monster {
    Uktril    = 1,
    Geraktril = 2,
    Reishid   = 3,
    Custom    = 4,
};

static Entity MonsterFactory(Monster monsterType);

}  // namespace factory
}  // namespace internal

#endif  // SRC_INTERNAL_FACTORY_MONSTER_FACTORY_H_

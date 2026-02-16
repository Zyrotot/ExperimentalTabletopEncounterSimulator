// -----------------------------------------------------------------------------
// | @file      factory.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_FACTORY_FACTORY_H_
#define SRC_INTERNAL_FACTORY_FACTORY_H_

#include <string>

#include "internal/entities/entity.h"

namespace internal {
namespace factory {

using entities::Entity;
using entities::EntityConfig;

enum Monster {
    Uktril    = 1,
    Geraktril = 2,
    Reishid   = 3,
    Custom    = 4,
};

Entity MonsterFactory(Monster monsterType);
Entity GetPlayer(const std::string& filename);

EntityConfig LoadCharacterFromJSON(const std::string& filename);

}  // namespace factory
}  // namespace internal

#endif  // SRC_INTERNAL_FACTORY_FACTORY_H_

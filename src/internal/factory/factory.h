// -----------------------------------------------------------------------------
// | @file      factory.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_FACTORY_FACTORY_H_
#define SRC_INTERNAL_FACTORY_FACTORY_H_

#include <memory>
#include <string>

#include "internal/entities/entity.h"

namespace internal {
namespace factory {

enum Monster {
    Uktril    = 1,
    Geraktril = 2,
    Reishid   = 3,
    Custom    = 4,
};

std::shared_ptr<entities::Entity> MonsterFactory(Monster monsterType);
std::shared_ptr<entities::Entity> GetPlayer(const std::string& filename);

entities::EntityConfig LoadCharacterFromJSON(const std::string& filename);

items::Enchantment RebuildEnchantmentFromName(const std::string& name);
abilities::Ability RebuildAbilityFromName(const std::string& name, int stack_count = 0);


}  // namespace factory
}  // namespace internal

#endif  // SRC_INTERNAL_FACTORY_FACTORY_H_

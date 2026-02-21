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
#include "internal/factory/i_factory.h"

namespace internal {
namespace factory {

enum Monster {
  Uktril = 1,
  Geraktril = 2,
  Reishid = 3,
  Custom = 4,
};

class Factory : public IFactory {
 public:
  Factory(std::string player_filename, Monster monster_type);

  std::shared_ptr<entities::IEntity> CreatePlayer() const override;
  std::shared_ptr<entities::IEntity> CreateMonster() const override;

 private:
  std::shared_ptr<entities::Entity> MonsterFactory(Monster monsterType) const;
  std::shared_ptr<entities::Entity> GetCharacterFromJSON(
      const std::string& filename) const;

  entities::EntityConfig LoadCharacterFromJSON(const std::string& filename) const;

  std::shared_ptr<entities::Entity> CreateExampleCharacter() const;
  std::shared_ptr<entities::Entity> CreateCustomEnemy() const;

  void SaveCharacterToJSON(const entities::EntityConfig& character,
                           const std::string& filename) const;

  items::Enchantment RebuildEnchantmentFromName(const std::string& name) const;
  abilities::Ability RebuildAbilityFromName(const std::string& name,
                                            int stack_count = 0) const;

  std::string player_filename_;
  Monster monster_type_;
};

}  // namespace factory
}  // namespace internal

#endif  // SRC_INTERNAL_FACTORY_FACTORY_H_

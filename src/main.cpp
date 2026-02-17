// -----------------------------------------------------------------------------
// | @file      main.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <iostream>

#include "internal/factory/factory.h"
#include "internal/logging/log_manager.h"
#include "internal/resolver/attack_resolver.h"
#include "internal/resolver/damage_resolver.h"

int main() {
  // for debugging purposes, set log level to debug for attack logger
  auto logger = internal::logging::LogManager::GetLogger("attack");
  logger->SetLogLevel(internal::logging::LogLevel::DEBUG);
  auto entity_logger = internal::logging::LogManager::GetLogger("entity");
  entity_logger->SetLogLevel(internal::logging::LogLevel::DEBUG);

  auto player = internal::factory::GetPlayer("character.json");
  auto uktril =
      internal::factory::MonsterFactory(internal::factory::Monster::Uktril);
  auto reishid =
      internal::factory::MonsterFactory(internal::factory::Monster::Reishid);

  auto roller = std::make_shared<internal::dice_rolls::Roller>();
  auto resolver = std::make_shared<internal::resolver::AttackResolver>(
      player, uktril, 0, roller);

  auto context = resolver->ResolveAttack();

  auto damage_resolver =
      std::make_shared<internal::resolver::DamageResolver>(context);
  damage_resolver->ApplyDamage();

  std::cout << uktril->GetName() << " has "
            << uktril->GetCurrentStats().base_stats.hp << " HP left."
            << std::endl;

  std::cout << player->GetName() << " has "
            << player->GetCurrentStats().base_stats.hp << " HP left and "
            << player->GetCurrentStats().bonus_stats.temporary_hp
            << " temporary HP left." << std::endl;

context = resolver->ResolveAttack();

  damage_resolver =
      std::make_shared<internal::resolver::DamageResolver>(context);
  damage_resolver->ApplyDamage();

  std::cout << uktril->GetName() << " has "
            << uktril->GetCurrentStats().base_stats.hp << " HP left."
            << std::endl;

  std::cout << player->GetName() << " has "
            << player->GetCurrentStats().base_stats.hp << " HP left and "
            << player->GetCurrentStats().bonus_stats.temporary_hp
            << " temporary HP left." << std::endl;

  auto resolver2 = std::make_shared<internal::resolver::AttackResolver>(
      reishid, player, 0, roller);

  auto context2 = resolver2->ResolveAttack();

  auto damage_resolver2 =
      std::make_shared<internal::resolver::DamageResolver>(context2);
  damage_resolver2->ApplyDamage();

  std::cout << player->GetName() << " has "
            << player->GetCurrentStats().base_stats.hp << " HP left and "
            << player->GetCurrentStats().bonus_stats.temporary_hp
            << " temporary HP left." << std::endl;

  return 0;
}

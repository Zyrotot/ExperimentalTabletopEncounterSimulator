// -----------------------------------------------------------------------------
// | @file      main.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <iostream>

#include "internal/dice_rolls/roller.h"
#include "internal/engine/ai_director.h"
#include "internal/engine/combat_engine.h"
#include "internal/engine/encounter.h"
#include "internal/factory/factory.h"
#include "internal/logging/log_manager.h"

int main() {
  internal::logging::LogManager::GetLogger("attack")
      ->SetLogLevel(internal::logging::LogLevel::DEBUG);
  internal::logging::LogManager::GetLogger("entity")
      ->SetLogLevel(internal::logging::LogLevel::DEBUG);
  internal::logging::LogManager::GetLogger("engine")
      ->SetLogLevel(internal::logging::LogLevel::DEBUG);
  internal::logging::LogManager::GetLogger("director")
      ->SetLogLevel(internal::logging::LogLevel::DEBUG);

  auto player = internal::factory::GetPlayer("character.json");
  auto uktril =
      internal::factory::MonsterFactory(internal::factory::Monster::Uktril);
  auto reishid =
      internal::factory::MonsterFactory(internal::factory::Monster::Reishid);

  auto roller = std::make_shared<internal::dice_rolls::Roller>();

  internal::engine::CombatEngine engine(roller);
  internal::engine::Encounter encounter({player}, {uktril, reishid});
  internal::engine::AIDirector director(&encounter, &engine);

  director.RunEncounter();

  return 0;
}

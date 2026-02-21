// -----------------------------------------------------------------------------
// | @file      main.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/dice_rolls/roller.h"
#include "internal/factory/factory.h"
#include "internal/simulator/simulator.h"

int main() {
  auto roller = std::make_shared<internal::dice_rolls::Roller>();

  auto entity_factory = std::make_unique<internal::factory::Factory>(
      "character.json", internal::factory::Monster::Reishid);

  internal::simulator::Simulator sim(std::move(entity_factory), roller);

  auto results = sim.Run(100);
  results.Print();

  return 0;
}

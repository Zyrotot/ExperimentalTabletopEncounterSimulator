// -----------------------------------------------------------------------------
// | @file      main.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/dice_rolls/roller.h"
#include "internal/factory/factory.h"
#include "internal/simulator/simulator.h"

int main() {
  auto roller = std::make_shared<ettes::dice_rolls::Roller>();

  auto entity_factory = std::make_unique<ettes::factory::Factory>(
      "character.json", ettes::factory::Monster::Custom);

  ettes::simulator::Simulator sim(std::move(entity_factory), roller);

  auto results = sim.Run(1000);
  results.Print();

  return 0;
}

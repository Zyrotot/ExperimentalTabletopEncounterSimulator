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

  internal::simulator::Simulator sim(
      "character.json",
      internal::factory::Monster::Geraktril,
      roller);

  auto results = sim.Run(1);
  results.Print();

  return 0;
}

// -----------------------------------------------------------------------------
// | @file      main.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <chrono>  // NOLINT
#include <iomanip>
#include <iostream>

#include "internal/dice_rolls/roller.h"
#include "internal/factory/factory.h"
#include "internal/simulator/simulator.h"

constexpr int kNumSimulations = 1000;

int main() {
  auto roller = std::make_shared<ettes::dice_rolls::Roller>();

  auto entity_factory = std::make_unique<ettes::factory::Factory>(
      "character.json", ettes::factory::Monster::Reishid);

  ettes::simulator::Simulator sim(std::move(entity_factory), roller);

  const auto t_start = std::chrono::steady_clock::now();
  auto results = sim.Run(kNumSimulations);
  const auto t_end = std::chrono::steady_clock::now();

  results.Print();

  const double elapsed_s =
      std::chrono::duration<double>(t_end - t_start).count();
  const double sims_per_sec = static_cast<double>(kNumSimulations) / elapsed_s;

  std::cout << std::fixed << std::setprecision(3);
  std::cout << "  Elapsed       : " << elapsed_s << " s\n";
  std::cout << std::setprecision(0);
  std::cout << "  Sims/sec      : " << sims_per_sec << "\n\n";

  return 0;
}

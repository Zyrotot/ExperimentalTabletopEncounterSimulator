// -----------------------------------------------------------------------------
// | @file      simulator.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/simulator/simulator.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>

#include "internal/dice_rolls/roller.h"
#include "internal/engine/combat_engine.h"
#include "internal/engine/director.h"
#include "internal/engine/encounter.h"
#include "internal/entities/entity.h"
#include "internal/factory/factory.h"
#include "internal/logging/log_manager.h"

#define MAX_WAVES 100

namespace internal {
namespace simulator {

namespace {

constexpr const char* kSimLoggers[] = {"attack", "entity", "engine",
                                       "director"};

void SetLoggers(logging::LogLevel level) {
  for (const auto* name : kSimLoggers) {
    logging::LogManager::GetLogger(name)->SetLogLevel(level);
  }
}

}  // namespace

// -----------------------------------------------------------------------------
// SimulationResults
// -----------------------------------------------------------------------------

void SimulationResults::Print() const {
  std::cout
      << "\n============================================================\n";
  std::cout << "  Simulation Results\n";
  std::cout << "============================================================\n";
  std::cout << "  Runs          : " << num_simulations << "\n";
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "  Avg waves     : " << average << "\n";
  std::cout << "  Min waves     : " << min_wave << "\n";
  std::cout << "  Max waves     : " << max_wave << "\n";
  std::cout << "\n  Wave distribution (waves cleared -> number of runs):\n";
  std::cout << "  ----------------------------------------------------------\n";
  for (const auto& [wave, count] : wave_distribution) {
    double pct = 100.0 * static_cast<double>(count) /
                 static_cast<double>(num_simulations);
    std::cout << "    Wave " << std::setw(3) << wave << " : " << std::setw(5)
              << count << " runs  (" << std::setw(6) << std::setprecision(2)
              << pct << "%)\n";
  }
  std::cout
      << "============================================================\n\n";
}

// -----------------------------------------------------------------------------
// Simulator
// -----------------------------------------------------------------------------

Simulator::Simulator(std::string player_filename, factory::Monster monster_type,
                     std::shared_ptr<dice_rolls::Roller> roller)
    : player_filename_(std::move(player_filename)),
      monster_type_(monster_type),
      roller_(std::move(roller)),
      logger_(logging::LogManager::GetLogger("simulator")) {
}

SimulationResults Simulator::Run(int num_simulations) const {
  SetLoggers(logging::LogLevel::INFO);

  SimulationResults results;
  results.num_simulations = num_simulations;
  results.max_waves_cleared.reserve(static_cast<std::size_t>(num_simulations));

  for (int i = 0; i < num_simulations; ++i) {
    const int cleared = RunOnce();
    results.max_waves_cleared.push_back(cleared);
    results.wave_distribution[cleared]++;
  }

  const auto& waves = results.max_waves_cleared;
  results.min_wave = *std::min_element(waves.begin(), waves.end());
  results.max_wave = *std::max_element(waves.begin(), waves.end());
  results.average =
      static_cast<double>(std::accumulate(waves.begin(), waves.end(), 0)) /
      static_cast<double>(num_simulations);

  return results;
}

int Simulator::RunOnce() const {
  int waves_cleared = 0;

  for (int wave = 1; wave <= MAX_WAVES; ++wave) {
    auto player = factory::GetPlayer(player_filename_);

    std::vector<std::shared_ptr<entities::Entity>> enemies;
    enemies.reserve(static_cast<std::size_t>(wave));
    for (int i = 0; i < wave; ++i) {
      enemies.push_back(factory::MonsterFactory(monster_type_));
    }

    engine::CombatEngine combat_engine(roller_);
    engine::Encounter encounter({player}, std::move(enemies));
    engine::Director director(&encounter, &combat_engine);

    director.RunEncounter();

    if (encounter.HasLivingEntitiesOnSideA()) {
      waves_cleared = wave;
      if (wave == MAX_WAVES) {
        logger_->Info("Wave cap ({}) reached — character wins!", MAX_WAVES);
        break;
      }
      logger_->Info("Wave {} cleared.", wave);
    } else {
      logger_->Info("Lost on wave {}.", wave);
      break;
    }
  }

  return waves_cleared;
}

}  // namespace simulator
}  // namespace internal

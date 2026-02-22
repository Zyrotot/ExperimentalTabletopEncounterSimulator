// -----------------------------------------------------------------------------
// | @file      simulator.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/simulator/simulator.h"

#include <algorithm>
#include <future>   // NOLINT
#include <iomanip>
#include <iostream>
#include <numeric>
#include <thread>  // NOLINT

#include "internal/dice_rolls/roller.h"
#include "internal/engine/combat_engine.h"
#include "internal/engine/director.h"
#include "internal/engine/encounter.h"
#include "internal/entities/i_entity.h"
#include "internal/factory/i_factory.h"
#include "internal/logging/log_manager.h"

constexpr int MAX_WAVES = 75;

namespace ettes {
namespace simulator {

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

Simulator::Simulator(std::unique_ptr<factory::IFactory> entity_factory,
                     std::shared_ptr<dice_rolls::Roller> roller)
    : entity_factory_(std::move(entity_factory)),
      roller_(std::move(roller)),
      logger_(logging::LogManager::GetLogger("simulator")) {
}

SimulationResults Simulator::Run(int num_simulations,
                                 unsigned int num_threads) const {
  if (num_threads == 0) {
    num_threads = std::max(
        1u, static_cast<unsigned int>(std::thread::hardware_concurrency()));
  }
  num_threads =
      std::min(num_threads, static_cast<unsigned int>(num_simulations));

  const int base = num_simulations / static_cast<int>(num_threads);
  const int remainder = num_simulations % static_cast<int>(num_threads);

  std::vector<std::future<std::vector<int>>> futures;
  futures.reserve(num_threads);

  for (unsigned int t = 0; t < num_threads; ++t) {
    const int count = base + (static_cast<int>(t) < remainder ? 1 : 0);

    futures.emplace_back(
        std::async(std::launch::async, [this, count]() {
          auto local_roller = std::make_shared<dice_rolls::Roller>();
          std::vector<int> results;
          results.reserve(static_cast<std::size_t>(count));

          for (int sim = 0; sim < count; ++sim) {
            results.push_back(RunOnce(local_roller));
          }
          return results;
        }));
  }

  SimulationResults results;
  results.num_simulations = num_simulations;
  results.max_waves_cleared.reserve(static_cast<std::size_t>(num_simulations));

  for (auto& f : futures) {
    for (const int v : f.get()) {
      results.max_waves_cleared.push_back(v);
      results.wave_distribution[v]++;
    }
  }

  const auto& waves = results.max_waves_cleared;
  results.min_wave = *std::min_element(waves.begin(), waves.end());
  results.max_wave = *std::max_element(waves.begin(), waves.end());
  results.average =
      static_cast<double>(std::accumulate(waves.begin(), waves.end(), 0)) /
      static_cast<double>(num_simulations);

  return results;
}

bool Simulator::RunWave(int wave,
                        std::shared_ptr<dice_rolls::Roller> roller) const {
  auto player = entity_factory_->CreatePlayer();

  std::vector<std::shared_ptr<entities::IEntity>> enemies;
  enemies.reserve(static_cast<std::size_t>(wave));
  for (int i = 0; i < wave; ++i) {
    enemies.push_back(entity_factory_->CreateMonster());
  }

  engine::CombatEngine combat_engine(roller);
  engine::Encounter encounter({player}, std::move(enemies));
  engine::Director director(&encounter, &combat_engine);

  director.RunEncounter();

  return encounter.HasLivingEntitiesOnSideA();
}

int Simulator::RunOnce(std::shared_ptr<dice_rolls::Roller> roller) const {
  int waves_cleared = 0;

  for (int wave = 1; wave <= MAX_WAVES; ++wave) {
    if (RunWave(wave, roller)) {
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
}  // namespace ettes

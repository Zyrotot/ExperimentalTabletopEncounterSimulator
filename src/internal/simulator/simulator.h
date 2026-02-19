// -----------------------------------------------------------------------------
// | @file      simulator.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_SIMULATOR_SIMULATOR_H_
#define SRC_INTERNAL_SIMULATOR_SIMULATOR_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "internal/factory/factory.h"
#include "internal/logging/logger.h"

namespace internal {

namespace dice_rolls {
class Roller;
}  // namespace dice_rolls

namespace simulator {

struct SimulationResults {
  int num_simulations = 0;

  std::vector<int> max_waves_cleared;

  double average  = 0.0;
  int    min_wave = 0;
  int    max_wave = 0;

  std::map<int, int> wave_distribution;

  void Print() const;
};

class Simulator {
 public:
  Simulator(std::string player_filename, factory::Monster monster_type,
            std::shared_ptr<dice_rolls::Roller> roller);

  SimulationResults Run(int num_simulations) const;

 private:
  int RunOnce() const;

  std::string player_filename_;
  factory::Monster monster_type_;
  std::shared_ptr<dice_rolls::Roller> roller_;
  std::shared_ptr<logging::Logger> logger_;
};

}  // namespace simulator
}  // namespace internal

#endif  // SRC_INTERNAL_SIMULATOR_SIMULATOR_H_

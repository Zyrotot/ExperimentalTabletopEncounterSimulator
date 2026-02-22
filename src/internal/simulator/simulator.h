// -----------------------------------------------------------------------------
// | @file      simulator.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_SIMULATOR_SIMULATOR_H_
#define SRC_INTERNAL_SIMULATOR_SIMULATOR_H_

#include <map>
#include <memory>
#include <vector>

#include "internal/factory/i_factory.h"
#include "internal/logging/logger.h"

namespace ettes {

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
  Simulator(std::unique_ptr<factory::IFactory> entity_factory,
            std::shared_ptr<dice_rolls::Roller> roller);

  SimulationResults Run(int num_simulations,
                        unsigned int num_threads = 0) const;

 private:
  bool RunWave(int wave, std::shared_ptr<dice_rolls::Roller> roller) const;

  int RunOnce(std::shared_ptr<dice_rolls::Roller> roller) const;

  std::unique_ptr<factory::IFactory> entity_factory_;
  std::shared_ptr<dice_rolls::Roller> roller_;
  logging::Logger* logger_;
};

}  // namespace simulator
}  // namespace ettes

#endif  // SRC_INTERNAL_SIMULATOR_SIMULATOR_H_

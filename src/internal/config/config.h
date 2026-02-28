// -----------------------------------------------------------------------------
// | @file      config.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_CONFIG_CONFIG_H_
#define SRC_INTERNAL_CONFIG_CONFIG_H_

#include <string>

namespace ettes {
namespace config {

enum class SimulationMode {
  WaveScaling,
  Endurance,
};

struct Config {
  SimulationMode simulation_mode = SimulationMode::WaveScaling;
  int num_simulations = 1;
  int max_waves = 75;
  std::string player_file = "character.json";
  std::string monster = "reishid";
  std::string log_level = "info";
  int num_threads = 0;
};

Config LoadConfig(const std::string& path);

}  // namespace config
}  // namespace ettes

#endif  // SRC_INTERNAL_CONFIG_CONFIG_H_

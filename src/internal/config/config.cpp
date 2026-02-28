// -----------------------------------------------------------------------------
// | @file      config.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/config/config.h"

#include <fstream>
#include <string>

#include "internal/logging/log_manager.h"

#define GLZ_USE_STD_FORMAT_FLOAT 0
#include <glaze/glaze.hpp>

template <>
struct glz::meta<ettes::config::SimulationMode> {
  using enum ettes::config::SimulationMode;
  static constexpr auto value =
      glz::enumerate("wave_scaling", WaveScaling, "endurance", Endurance);
};

namespace ettes {
namespace config {

Config LoadConfig(const std::string& path) {
  auto* logger = logging::LogManager::GetLogger("config");

  std::string file_contents;
  {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) {
      logger->Warn("Config file not found: {}. Using defaults.", path);
      return Config{};
    }
    file_contents.assign(std::istreambuf_iterator<char>(ifs),
                         std::istreambuf_iterator<char>());
  }

  Config cfg{};
  auto ec = glz::read_json(cfg, file_contents);
  if (ec) {
    logger->Error("Failed to parse config JSON: {}",
                  glz::format_error(ec, file_contents));
    return Config{};
  }

  logger->Info("Config loaded from {}", path);
  return cfg;
}

}  // namespace config
}  // namespace ettes

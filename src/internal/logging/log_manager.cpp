// -----------------------------------------------------------------------------
// | @file      log_manager.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/logging/log_manager.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <mutex>  // NOLINT

namespace internal {
namespace logging {

std::shared_ptr<Logger> LogManager::GetLogger(const std::string& name) {
  static std::mutex creation_mutex;
  std::lock_guard<std::mutex> lock(creation_mutex);

  auto spd = spdlog::get(name);
  if (!spd) {
    spd = spdlog::stdout_color_mt(name);
    spd->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");
  }
  return std::shared_ptr<Logger>(new Logger(spd));
}

}  // namespace logging
}  // namespace internal

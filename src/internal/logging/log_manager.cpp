// -----------------------------------------------------------------------------
// | @file      log_manager.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/logging/log_manager.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <mutex>  // NOLINT
#include <unordered_map>

#include "spdlog/common.h"

namespace ettes {
namespace logging {

Logger* LogManager::GetLogger(const std::string& name) {
  static std::mutex creation_mutex;
  static std::unordered_map<std::string, std::unique_ptr<Logger>> wrappers;
  std::lock_guard<std::mutex> lock(creation_mutex);

  auto wit = wrappers.find(name);
  if (wit != wrappers.end()) {
    return wit->second.get();
  }

  auto spd = spdlog::get(name);
  if (!spd) {
    spd = spdlog::stdout_color_mt(name);
    spd->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");
  }

  auto [it, ok] = wrappers.emplace(name, std::make_unique<Logger>(spd.get()));
  return it->second.get();
}

}  // namespace logging
}  // namespace ettes

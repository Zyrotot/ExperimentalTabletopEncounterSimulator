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
#include <string>
#include <unordered_map>

namespace ettes {
namespace logging {

spdlog::level::level_enum LogManager::default_level_ = spdlog::level::info;

void LogManager::SetDefaultLevel(const std::string& level) {
  default_level_ = ParseLevel(level);
}

spdlog::level::level_enum LogManager::ParseLevel(const std::string& level) {
  if (level == "trace") {
    return spdlog::level::trace;
  }
  if (level == "debug") {
    return spdlog::level::debug;
  }
  if (level == "info") {
    return spdlog::level::info;
  }
  if (level == "warn") {
    return spdlog::level::warn;
  }
  if (level == "error") {
    return spdlog::level::err;
  }
  if (level == "critical") {
    return spdlog::level::critical;
  }
  return spdlog::level::info;
}

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
    spd->set_level(default_level_);
  }

  auto [it, ok] = wrappers.emplace(name, std::make_unique<Logger>(spd.get()));
  return it->second.get();
}

}  // namespace logging
}  // namespace ettes

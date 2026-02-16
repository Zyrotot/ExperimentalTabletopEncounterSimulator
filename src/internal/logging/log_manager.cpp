// -----------------------------------------------------------------------------
// | @file      log_manager.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/logging/log_manager.h"

namespace internal {
namespace logging {

LogManager& LogManager::Instance() {
  static LogManager instance;
  return instance;
}

std::shared_ptr<Logger> LogManager::GetLogger(const std::string& name) {
  return Instance().GetLoggerInternal(name);
}

std::shared_ptr<Logger> LogManager::GetLoggerInternal(const std::string& name) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = loggers_.find(name);
  if (it != loggers_.end()) {
    return it->second;
  }

  auto logger = std::shared_ptr<Logger>(new Logger(name));
  loggers_[name] = logger;
  return logger;
}

}  // namespace logging
}  // namespace internal

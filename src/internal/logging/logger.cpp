// -----------------------------------------------------------------------------
// | @file      logger.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/logging/logger.h"

#include <spdlog/logger.h>

namespace internal {
namespace logging {

Logger::Logger(std::shared_ptr<spdlog::logger> spd) : spd_(std::move(spd)) {
}

void Logger::Debug(const std::string& msg) {
  spd_->debug(msg);
}
void Logger::Info(const std::string& msg) {
  spd_->info(msg);
}
void Logger::Warning(const std::string& msg) {
  spd_->warn(msg);
}
void Logger::Error(const std::string& msg) {
  spd_->error(msg);
}

const std::string& Logger::GetName() const {
  return spd_->name();
}

void Logger::SetLogLevel(LogLevel level) {
  spd_->set_level(level);
}

LogLevel Logger::GetLogLevel() const {
  return spd_->level();
}

}  // namespace logging
}  // namespace internal

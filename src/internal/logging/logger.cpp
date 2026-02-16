// -----------------------------------------------------------------------------
// | @file      logger.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/logging/logger.h"

#include <chrono>  // NOLINT
#include <ctime>
#include <iomanip>
#include <iostream>

namespace internal {
namespace logging {

Logger::Logger(const std::string& name) : name_(name) {
}

void Logger::Debug(const std::string& message) {
  Log(LogLevel::DEBUG, message);
}

void Logger::Info(const std::string& message) {
  Log(LogLevel::INFO, message);
}

void Logger::Warning(const std::string& message) {
  Log(LogLevel::WARNING, message);
}

void Logger::Error(const std::string& message) {
  Log(LogLevel::ERROR, message);
}

const std::string& Logger::GetName() const {
  return name_;
}

void Logger::SetLogLevel(LogLevel level) {
  min_level_ = level;
}

LogLevel Logger::GetLogLevel() const {
  return min_level_;
}

void Logger::Log(LogLevel level, const std::string& message) {
  if (level < min_level_) {
    return;
  }

  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) %
            1000;

  std::tm tm_info;
  localtime_r(&time, &tm_info);

  std::cout << "[" << std::put_time(&tm_info, "%Y-%m-%d %H:%M:%S") << "."
            << std::setfill('0') << std::setw(3) << ms.count() << "] "
            << "[" << LogLevelToString(level) << "] "
            << "[" << name_ << "] " << message << std::endl;
}

const char* Logger::LogLevelToString(LogLevel level) const {
  switch (level) {
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARNING:
      return "WARN";
    case LogLevel::ERROR:
      return "ERROR";
    default:
      return "UNKN";
  }
}

}  // namespace logging
}  // namespace internal

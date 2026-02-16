// -----------------------------------------------------------------------------
// | @file      log_manager.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_LOGGING_LOG_MANAGER_H_
#define SRC_INTERNAL_LOGGING_LOG_MANAGER_H_

#include <map>
#include <memory>
#include <mutex>  // NOLINT
#include <string>

#include "internal/logging/logger.h"

namespace internal {
namespace logging {

class LogManager {
 public:
  static std::shared_ptr<Logger> GetLogger(const std::string& name);

 private:
  LogManager() = default;
  static LogManager& Instance();

  std::shared_ptr<Logger> GetLoggerInternal(const std::string& name);

  std::map<std::string, std::shared_ptr<Logger>> loggers_;
  std::mutex mutex_;
};

}  // namespace logging
}  // namespace internal

#endif  // SRC_INTERNAL_LOGGING_LOG_MANAGER_H_

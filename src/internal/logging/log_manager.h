// -----------------------------------------------------------------------------
// | @file      log_manager.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_LOGGING_LOG_MANAGER_H_
#define SRC_INTERNAL_LOGGING_LOG_MANAGER_H_

#include <memory>
#include <string>

#include "internal/logging/logger.h"

namespace internal {
namespace logging {

class LogManager {
 public:
  static std::shared_ptr<Logger> GetLogger(const std::string& name);

  LogManager()  = delete;
  ~LogManager() = delete;
};

}  // namespace logging
}  // namespace internal

#endif  // SRC_INTERNAL_LOGGING_LOG_MANAGER_H_

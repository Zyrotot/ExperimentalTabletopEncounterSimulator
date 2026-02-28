// -----------------------------------------------------------------------------
// | @file      log_manager.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_LOGGING_LOG_MANAGER_H_
#define SRC_INTERNAL_LOGGING_LOG_MANAGER_H_

#include <spdlog/common.h>

#include <string>

#include "internal/logging/logger.h"

namespace ettes {
namespace logging {

class LogManager {
 public:
  static Logger* GetLogger(const std::string& name);
  static void SetDefaultLevel(const std::string& level);

  LogManager()  = delete;
  ~LogManager() = delete;

 private:
  static spdlog::level::level_enum ParseLevel(const std::string& level);
  static spdlog::level::level_enum default_level_;
};

}  // namespace logging
}  // namespace ettes

#endif  // SRC_INTERNAL_LOGGING_LOG_MANAGER_H_

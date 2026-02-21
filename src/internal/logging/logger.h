// -----------------------------------------------------------------------------
// | @file      logger.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_LOGGING_LOGGER_H_
#define SRC_INTERNAL_LOGGING_LOGGER_H_

#include <spdlog/logger.h>

#include <memory>
#include <string>
#include <utility>

namespace internal {
namespace logging {

using LogLevel = spdlog::level::level_enum;

class Logger {
 public:
  void Debug(const std::string& msg);
  void Info(const std::string& msg);
  void Warning(const std::string& msg);
  void Error(const std::string& msg);

  template <typename... Args>
  void Debug(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spd_->debug(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void Info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spd_->info(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void Warning(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spd_->warn(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void Error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spd_->error(fmt, std::forward<Args>(args)...);
  }

  const std::string& GetName() const;
  void SetLogLevel(LogLevel level);
  LogLevel GetLogLevel() const;

 private:
  friend class LogManager;
  explicit Logger(std::shared_ptr<spdlog::logger> spd);

  std::shared_ptr<spdlog::logger> spd_;
};

}  // namespace logging
}  // namespace internal

#endif  // SRC_INTERNAL_LOGGING_LOGGER_H_

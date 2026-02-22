// -----------------------------------------------------------------------------
// | @file      logger.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_LOGGING_LOGGER_H_
#define SRC_INTERNAL_LOGGING_LOGGER_H_

#include <spdlog/common.h>
#include <spdlog/logger.h>

#include <utility>

namespace ettes {
namespace logging {

using LogLevel = spdlog::level::level_enum;

class Logger {
 public:
  explicit Logger(spdlog::logger* impl);

  template <typename... Args>
  void Debug([[maybe_unused]] spdlog::format_string_t<Args...> fmt,
             [[maybe_unused]] Args&&... args) const {
#ifndef NDEBUG
    impl_->debug(fmt, std::forward<Args>(args)...);
#endif
  }

  template <typename... Args>
  void Info([[maybe_unused]] spdlog::format_string_t<Args...> fmt,
            [[maybe_unused]] Args&&... args) const {
#ifndef NDEBUG
    impl_->info(fmt, std::forward<Args>(args)...);
#endif
  }

  template <typename... Args>
  void Warn([[maybe_unused]] spdlog::format_string_t<Args...> fmt,
            [[maybe_unused]] Args&&... args) const {
    impl_->warn(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void Error([[maybe_unused]] spdlog::format_string_t<Args...> fmt,
             [[maybe_unused]] Args&&... args) const {
    impl_->error(fmt, std::forward<Args>(args)...);
  }

 private:
  spdlog::logger* impl_;
};

}  // namespace logging
}  // namespace ettes

#endif  // SRC_INTERNAL_LOGGING_LOGGER_H_

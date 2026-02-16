// -----------------------------------------------------------------------------
// | @file      logger.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_LOGGING_LOGGER_H_
#define SRC_INTERNAL_LOGGING_LOGGER_H_

#include <sstream>
#include <string>

namespace internal {
namespace logging {

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

// Simple formatting helper
namespace detail {
inline void FormatImpl(std::ostringstream& oss, const std::string& fmt,
                       size_t pos) {
  // Base case: append remaining string
  oss << fmt.substr(pos);
}

template <typename T, typename... Args>
void FormatImpl(std::ostringstream& oss, const std::string& fmt, size_t pos,
                const T& value, const Args&... args) {
  size_t placeholder = fmt.find("{}", pos);
  if (placeholder == std::string::npos) {
    oss << fmt.substr(pos);
    return;
  }
  oss << fmt.substr(pos, placeholder - pos);
  oss << value;
  FormatImpl(oss, fmt, placeholder + 2, args...);
}

template <typename... Args>
std::string Format(const std::string& fmt, const Args&... args) {
  std::ostringstream oss;
  FormatImpl(oss, fmt, 0, args...);
  return oss.str();
}
}  // namespace detail

class Logger {
 public:
  void Debug(const std::string& message);
  void Info(const std::string& message);
  void Warning(const std::string& message);
  void Error(const std::string& message);

  template <typename... Args>
  void Debug(const std::string& fmt, const Args&... args) {
    Log(LogLevel::DEBUG, detail::Format(fmt, args...));
  }

  template <typename... Args>
  void Info(const std::string& fmt, const Args&... args) {
    Log(LogLevel::INFO, detail::Format(fmt, args...));
  }

  template <typename... Args>
  void Warning(const std::string& fmt, const Args&... args) {
    Log(LogLevel::WARNING, detail::Format(fmt, args...));
  }

  template <typename... Args>
  void Error(const std::string& fmt, const Args&... args) {
    Log(LogLevel::ERROR, detail::Format(fmt, args...));
  }

  const std::string& GetName() const;
  void SetLogLevel(LogLevel level);
  LogLevel GetLogLevel() const;

 private:
  friend class LogManager;
  explicit Logger(const std::string& name);

  void Log(LogLevel level, const std::string& message);
  const char* LogLevelToString(LogLevel level) const;

  std::string name_;
  LogLevel min_level_ = LogLevel::INFO;
};

}  // namespace logging
}  // namespace internal

#endif  // SRC_INTERNAL_LOGGING_LOGGER_H_

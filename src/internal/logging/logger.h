// -----------------------------------------------------------------------------
// | @file      logger.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_LOGGING_LOGGER_H_
#define SRC_INTERNAL_LOGGING_LOGGER_H_

#include <spdlog/logger.h>

namespace ettes {
namespace logging {

using LogLevel = spdlog::level::level_enum;
using Logger   = spdlog::logger;

}  // namespace logging
}  // namespace ettes

#endif  // SRC_INTERNAL_LOGGING_LOGGER_H_

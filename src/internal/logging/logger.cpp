// -----------------------------------------------------------------------------
// | @file      logger.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/logging/logger.h"

namespace ettes {
namespace logging {

Logger::Logger(spdlog::logger* impl) : impl_(impl) {
}

}  // namespace logging
}  // namespace ettes

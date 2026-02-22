// -----------------------------------------------------------------------------
// | @file      random_engine.cpp
// | @author    Copilot
// | @project   ETTES
// -----------------------------------------------------------------------------

#include "internal/dice_rolls/random_engine.h"

#include <random>

namespace ettes {
namespace dice_rolls {

PcgEngine::PcgEngine() {
  std::random_device rd;
  state_ = (static_cast<uint64_t>(rd()) << 32u) | rd();
  inc_ = ((static_cast<uint64_t>(rd()) << 32u) | rd()) | 1u;
  GetRand(1);
}

int PcgEngine::GetRand(int hi) {
  uint64_t old = state_;
  state_ = old * 6364136223846793005ULL + inc_;
  uint32_t xorshifted = static_cast<uint32_t>(((old >> 18u) ^ old) >> 27u);
  uint32_t rot = static_cast<uint32_t>(old >> 59u);
  uint32_t output = (xorshifted >> rot) | (xorshifted << ((-rot) & 31u));
  return static_cast<int>(output % static_cast<uint32_t>(hi)) + 1;
}

}  // namespace dice_rolls
}  // namespace ettes

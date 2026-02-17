// -----------------------------------------------------------------------------
// | @file      random_engine.cpp
// | @author    Copilot
// | @project   ETTES
// -----------------------------------------------------------------------------

#include "internal/dice_rolls/random_engine.h"

namespace internal {
namespace dice_rolls {

StdMt19937Engine::StdMt19937Engine() : rng_(std::random_device {}()) {
}

int StdMt19937Engine::GetRand(int hi) {
  std::uniform_int_distribution<> distr(1, hi);
  return distr(rng_);
}

}  // namespace dice_rolls
}  // namespace internal

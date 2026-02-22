// -----------------------------------------------------------------------------
// | @file      random_engine.cpp
// | @author    Copilot
// | @project   ETTES
// -----------------------------------------------------------------------------

#include "internal/dice_rolls/random_engine.h"

namespace ettes {
namespace dice_rolls {

StdMt19937Engine::StdMt19937Engine() : rng_(std::random_device {}()) {
}

int StdMt19937Engine::GetRand(int hi) {
  dist_.param(std::uniform_int_distribution<>::param_type(1, hi));
  return dist_(rng_);
}

}  // namespace dice_rolls
}  // namespace ettes

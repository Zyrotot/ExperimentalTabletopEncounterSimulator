// -----------------------------------------------------------------------------
// | @file      random_engine.h
// | @author    Copilot
// | @project   ETTES
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_DICE_ROLLS_RANDOM_ENGINE_H_
#define SRC_INTERNAL_DICE_ROLLS_RANDOM_ENGINE_H_

#include <random>

namespace internal {
namespace dice_rolls {

struct IRandomEngine {
  virtual int GetRand(int hi) = 0;
  virtual ~IRandomEngine() = default;
};

class StdMt19937Engine : public IRandomEngine {
 public:
  StdMt19937Engine();

  int GetRand(int hi) override;

 private:
  std::mt19937 rng_;
};

}  // namespace dice_rolls
}  // namespace internal

#endif  // SRC_INTERNAL_DICE_ROLLS_RANDOM_ENGINE_H_

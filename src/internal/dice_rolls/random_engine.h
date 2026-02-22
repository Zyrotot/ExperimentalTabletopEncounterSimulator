// -----------------------------------------------------------------------------
// | @file      random_engine.h
// | @author    Copilot
// | @project   ETTES
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_DICE_ROLLS_RANDOM_ENGINE_H_
#define SRC_INTERNAL_DICE_ROLLS_RANDOM_ENGINE_H_

#include <random>

namespace ettes {
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
  std::uniform_int_distribution<> dist_;
};

}  // namespace dice_rolls
}  // namespace ettes

#endif  // SRC_INTERNAL_DICE_ROLLS_RANDOM_ENGINE_H_

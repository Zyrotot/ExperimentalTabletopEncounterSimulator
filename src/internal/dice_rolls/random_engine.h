// -----------------------------------------------------------------------------
// | @file      random_engine.h
// | @author    Copilot
// | @project   ETTES
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_DICE_ROLLS_RANDOM_ENGINE_H_
#define SRC_INTERNAL_DICE_ROLLS_RANDOM_ENGINE_H_

#include <cstdint>

namespace ettes {
namespace dice_rolls {

struct IRandomEngine {
  virtual int GetRand(int hi) = 0;
  virtual ~IRandomEngine() = default;
};

class PcgEngine : public IRandomEngine {
 public:
  PcgEngine();

  int GetRand(int hi) override;

 private:
  uint64_t state_;
  uint64_t inc_;
};

}  // namespace dice_rolls
}  // namespace ettes

#endif  // SRC_INTERNAL_DICE_ROLLS_RANDOM_ENGINE_H_

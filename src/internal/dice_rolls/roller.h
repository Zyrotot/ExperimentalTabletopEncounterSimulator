// -----------------------------------------------------------------------------
// | @file      roller.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_DICE_ROLLS_ROLLER_H_
#define SRC_INTERNAL_DICE_ROLLS_ROLLER_H_

#include <memory>

#include "internal/dice_rolls/term.h"

namespace ettes {
namespace dice_rolls {

class IRandomEngine;

class Roller {
 public:
  Roller();
  explicit Roller(std::shared_ptr<IRandomEngine> engine);
  ~Roller();

  int Roll(const Term& term);

 private:
  std::shared_ptr<IRandomEngine> engine_;
};

}  // namespace dice_rolls
}  // namespace ettes

#endif  // SRC_INTERNAL_DICE_ROLLS_ROLLER_H_

// -----------------------------------------------------------------------------
// | @file      roller.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_DICE_ROLLS_ROLLER_H_
#define SRC_INTERNAL_DICE_ROLLS_ROLLER_H_

#include <memory>
#include <vector>

#include "internal/dice_rolls/random_engine.h"

namespace internal {
namespace dice_rolls {

struct Dice {
  int sides;
};

struct Term {
  std::vector<Dice> dices;
  int bonus;
};

struct Expression {
  std::vector<Term> terms;
};

class Roller {
 public:
  Roller();
  ~Roller();

  int Roll(const Term& term);

 protected:
  explicit Roller(std::shared_ptr<IRandomEngine> engine);

 private:
  std::shared_ptr<IRandomEngine> engine_;
};

}  // namespace dice_rolls
}  // namespace internal

#endif  // SRC_INTERNAL_DICE_ROLLS_ROLLER_H_

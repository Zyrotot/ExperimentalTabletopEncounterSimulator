// -----------------------------------------------------------------------------
// | @file      roller.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_DICE_ROLLS_ROLLER_H_
#define SRC_INTERNAL_DICE_ROLLS_ROLLER_H_

#include <memory>
#include <vector>

namespace internal {
namespace dice_rolls {

class IRandomEngine;

struct Dice {
  int count = 0;
  int sides = 0;
};

struct Term {
  std::vector<Dice> dice_groups = {};
  int bonus = 0;

  void AddModifier(const int& modifier);
};

inline Term operator+ (const Term& lhs, const Term& rhs) {
  Term result;
  result.dice_groups = lhs.dice_groups;
  result.dice_groups.insert(result.dice_groups.end(), rhs.dice_groups.begin(), rhs.dice_groups.end());
  result.bonus = lhs.bonus + rhs.bonus;
  return result;
}

inline void Term::AddModifier(const int& modifier) {
  bonus += modifier;
}

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

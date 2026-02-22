// -----------------------------------------------------------------------------
// | @file      term.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_DICE_ROLLS_TERM_H_
#define SRC_INTERNAL_DICE_ROLLS_TERM_H_

#include <optional>
#include <string>
#include <vector>

namespace ettes {
namespace dice_rolls {

struct Dice {
  int count = 0;
  int sides = 0;
};

class Term {
 public:
  std::vector<Dice> dice_groups = {};
  int bonus = 0;

  static std::optional<Term> Parse(const std::string& expression);

  void AddModifier(int modifier);
};

Term operator+(const Term& lhs, const Term& rhs);

}  // namespace dice_rolls
}  // namespace ettes

#endif  // SRC_INTERNAL_DICE_ROLLS_TERM_H_

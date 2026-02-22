// -----------------------------------------------------------------------------
// | @file      term.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/dice_rolls/term.h"

#include <regex>  // NOLINT

namespace internal {
namespace dice_rolls {

std::optional<Term> Term::Parse(const std::string& expression) {
  if (expression.empty()) {
    return std::nullopt;
  }

  std::regex dice_regex(R"((\d+)d(\d+))");
  auto begin = std::sregex_iterator(expression.begin(), expression.end(),
                                    dice_regex);
  auto end = std::sregex_iterator();

  if (begin == end) {
    return std::nullopt;
  }

  Term result;
  for (auto it = begin; it != end; ++it) {
    int count = std::stoi((*it)[1].str());
    int sides = std::stoi((*it)[2].str());
    result.dice_groups.push_back(Dice{count, sides});
  }

  std::string stripped = std::regex_replace(expression, dice_regex, "");
  std::regex bonus_regex(R"([+-]?\d+)");
  auto bbegin = std::sregex_iterator(stripped.begin(), stripped.end(),
                                     bonus_regex);
  for (auto it = bbegin; it != std::sregex_iterator(); ++it) {
    result.bonus += std::stoi((*it)[0].str());
  }

  return result;
}

Term operator+(const Term& lhs, const Term& rhs) {
  Term result;
  result.dice_groups.reserve(lhs.dice_groups.size() + rhs.dice_groups.size());
  result.dice_groups.insert(result.dice_groups.end(), lhs.dice_groups.begin(),
                            lhs.dice_groups.end());
  result.dice_groups.insert(result.dice_groups.end(), rhs.dice_groups.begin(),
                            rhs.dice_groups.end());
  result.bonus = lhs.bonus + rhs.bonus;
  return result;
}

void Term::AddModifier(int modifier) {
  bonus += modifier;
}

}  // namespace dice_rolls
}  // namespace internal

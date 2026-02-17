// -----------------------------------------------------------------------------
// | @file      alignment.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_RULES_ALIGNMENT_H_
#define SRC_INTERNAL_RULES_ALIGNMENT_H_

namespace internal {
namespace rules {

enum class Alignment {
  Good = 1 << 0,
  Neutral_M = 1 << 1,
  Evil = 1 << 2,

  Lawful = 1 << 3,
  Neutral_O = 1 << 4,
  Chaotic = 1 << 5,

  LawfulGood = Good | Lawful,
  NeutralGood = Good | Neutral_O,
  ChaoticGood = Good | Chaotic,

  LawfulNeutral = Neutral_M | Lawful,
  TrueNeutral = Neutral_M | Neutral_O,
  ChaoticNeutral = Neutral_M | Chaotic,

  LawfulEvil = Evil | Lawful,
  NeutralEvil = Evil | Neutral_O,
  ChaoticEvil = Evil | Chaotic,
};

}  // namespace rules
}  // namespace internal

#endif  // SRC_INTERNAL_RULES_ALIGNMENT_H_

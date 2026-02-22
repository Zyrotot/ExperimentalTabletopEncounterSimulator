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
  NeutralM = 1 << 1,
  Evil = 1 << 2,

  Lawful = 1 << 3,
  NeutralO = 1 << 4,
  Chaotic = 1 << 5,

  LawfulGood = Good | Lawful,
  NeutralGood = Good | NeutralO,
  ChaoticGood = Good | Chaotic,

  LawfulNeutral = NeutralM | Lawful,
  TrueNeutral = NeutralM | NeutralO,
  ChaoticNeutral = NeutralM | Chaotic,

  LawfulEvil = Evil | Lawful,
  NeutralEvil = Evil | NeutralO,
  ChaoticEvil = Evil | Chaotic,
};

}  // namespace rules
}  // namespace internal

#endif  // SRC_INTERNAL_RULES_ALIGNMENT_H_

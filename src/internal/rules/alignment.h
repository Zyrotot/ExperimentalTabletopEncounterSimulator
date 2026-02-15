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
    LawfulGood,     NeutralGood,    ChaoticGood,
    LawfulNeutral,  TrueNeutral,    ChaoticNeutral,
    LawfulEvil,     NeutralEvil,    ChaoticEvil,
};

}  // namespace rules
}  // namespace internal

#endif  // SRC_INTERNAL_RULES_ALIGNMENT_H_

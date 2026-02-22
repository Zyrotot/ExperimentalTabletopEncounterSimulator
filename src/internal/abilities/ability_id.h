// -----------------------------------------------------------------------------
// | @file      ability_id.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ABILITIES_ABILITY_ID_H_
#define SRC_INTERNAL_ABILITIES_ABILITY_ID_H_

#include <cstddef>
#include <cstdint>
#include <functional>

namespace ettes {
namespace abilities {

enum class AbilityId : uint8_t {
  None = 0,
  Erosao,
  RigidezRaivosa,
  Trespassar,
  DuroDeFerir,
  DuroDeMatar,
};

}  // namespace abilities
}  // namespace ettes

namespace std {
template <>
struct hash<ettes::abilities::AbilityId> {
  std::size_t operator()(ettes::abilities::AbilityId id) const noexcept {
    return std::hash<uint8_t>{}(static_cast<uint8_t>(id));
  }
};
}  // namespace std

#endif  // SRC_INTERNAL_ABILITIES_ABILITY_ID_H_

// -----------------------------------------------------------------------------
// | @file      entity_size.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/positioning/entity_size.h"

namespace ettes {
namespace positioning {

int FootprintCells(EntitySize size) {
  switch (size) {
    case EntitySize::Tiny:
      return 1;
    case EntitySize::Small:
      return 1;
    case EntitySize::Medium:
      return 1;
    case EntitySize::Large:
      return 2;
    case EntitySize::Huge:
      return 3;
    case EntitySize::Gargantuan:
      return 4;
  }
  return 1;
}

}  // namespace positioning
}  // namespace ettes

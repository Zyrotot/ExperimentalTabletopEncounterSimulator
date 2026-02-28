// -----------------------------------------------------------------------------
// | @file      entity_size.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_POSITIONING_ENTITY_SIZE_H_
#define SRC_INTERNAL_POSITIONING_ENTITY_SIZE_H_

namespace ettes {
namespace positioning {

enum class EntitySize {
  Tiny,
  Small,
  Medium,
  Large,
  Huge,
  Gargantuan
};

int FootprintCells(EntitySize size);

}  // namespace positioning
}  // namespace ettes

#endif  // SRC_INTERNAL_POSITIONING_ENTITY_SIZE_H_

// -----------------------------------------------------------------------------
// | @file      entity.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ENTITIES_ENTITY_H_
#define SRC_INTERNAL_ENTITIES_ENTITY_H_

#include "internal/entities/stats.h"
#include "internal/rules/alignment.h"

namespace internal {
namespace entities {

class Entity {
 public:
  Entity();
  virtual ~Entity();

 protected:
  rules::Alignment alignment;
  Stats starting_stats;
  Stats current_stats;
};

}  // namespace entities
}  // namespace internal

#endif  // SRC_INTERNAL_ENTITIES_ENTITY_H_

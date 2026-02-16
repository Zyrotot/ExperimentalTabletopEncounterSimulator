// -----------------------------------------------------------------------------
// | @file      entity.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/entities/entity.h"

namespace internal {
namespace entities {

Entity::Entity(const EntityConfig& config)
    : name_(config.name),
      starting_stats_(config.starting_stats),
      current_stats_(config.starting_stats),
      equipped_weapons_(config.equipped_weapons),
      attack_moves_(config.attack_moves),
      alignment_(config.alignment) {}

Entity::~Entity() {}

}  // namespace entities
}  // namespace internal

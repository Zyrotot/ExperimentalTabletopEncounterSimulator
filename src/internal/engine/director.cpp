// -----------------------------------------------------------------------------
// | @file      director.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/engine/director.h"

#include <cmath>
#include <cstddef>
#include <limits>
#include <string>
#include <vector>

#include "internal/combat/attack.h"
#include "internal/combat/event_manager.h"
#include "internal/entities/i_entity.h"
#include "internal/logging/log_manager.h"
#include "internal/positioning/auto_layout.h"
#include "internal/positioning/entity_size.h"

namespace ettes {
namespace engine {

Director::Director(Encounter* encounter, CombatEngine* engine,
                   positioning::DiagonalMode diagonal_mode)
    : encounter_(encounter),
      engine_(engine),
      logger_(logging::LogManager::GetLogger("director")),
      diagonal_mode_(diagonal_mode) {
  InitPositionMap();
}

Director::~Director() = default;

void Director::InitPositionMap() {
  std::vector<entities::IEntity*> side_a_ptrs;
  for (const auto& e : encounter_->GetSideA()) {
    if (e) {
      side_a_ptrs.push_back(e.get());
    }
  }
  std::vector<entities::IEntity*> side_b_ptrs;
  for (const auto& e : encounter_->GetSideB()) {
    if (e) {
      side_b_ptrs.push_back(e.get());
    }
  }

  int width = positioning::RecommendedGridWidth(side_a_ptrs, side_b_ptrs);
  int height = positioning::RecommendedGridHeight(side_a_ptrs, side_b_ptrs);

  position_map_ =
      std::make_unique<positioning::PositionMap>(width, height, diagonal_mode_);
  movement_resolver_ =
      std::make_unique<positioning::MovementResolver>(position_map_.get());

  positioning::AutoLayoutSides(position_map_.get(), side_a_ptrs, side_b_ptrs);

  LogGrid();
}

const positioning::PositionMap* Director::GetPositionMap() const {
  return position_map_.get();
}

void Director::RunEncounter() {
  auto collect_ids = [](const auto& side) {
    std::vector<uint32_t> ids;
    for (const auto& e : side) {
      if (e && e->IsAlive()) {
        ids.push_back(e->GetId());
      }
    }
    return ids;
  };

  while (!encounter_->IsOver()) {
    std::vector<uint32_t> side_a_ids = collect_ids(encounter_->GetSideA());
    std::vector<uint32_t> side_b_ids = collect_ids(encounter_->GetSideB());

    for (const auto& entity : encounter_->GetSideA()) {
      if (encounter_->IsOver()) {
        break;
      }
      if (!entity || !entity->IsAlive()) {
        continue;
      }
      ExecuteTurn(entity.get(), 0, side_a_ids);
    }
    for (const auto& entity : encounter_->GetSideB()) {
      if (encounter_->IsOver()) {
        break;
      }
      if (!entity || !entity->IsAlive()) {
        continue;
      }
      ExecuteTurn(entity.get(), 0, side_b_ids);
    }
  }

  logger_->Info("--- Encounter over ---");
  for (const auto& e : encounter_->GetSideA()) {
    logger_->Info("  [A] {} - {} HP", e->GetName(),
                  e->GetCurrentStats().base_stats.hp);
  }
  for (const auto& e : encounter_->GetSideB()) {
    logger_->Info("  [B] {} - {} HP", e->GetName(),
                  e->GetCurrentStats().base_stats.hp);
  }
}

void Director::RunTurn(entities::IEntity* entity, int attack_sequence_index) {
  ExecuteTurn(entity, attack_sequence_index, {});
}

void Director::ExecuteTurn(entities::IEntity* entity, int attack_sequence_index,
                           const std::vector<uint32_t>& ally_ids) {
  if (!entity || !entity->IsAlive()) {
    return;
  }

  auto target = SelectTarget(entity);
  if (!target) {
    logger_->Info("{} has no living targets, skipping turn", entity->GetName());
    return;
  }

  double weapon_range = GetMaxAttackRange(entity, attack_sequence_index);
  bool in_range =
      position_map_->GetDistanceMeters(entity, target) <= weapon_range;

  if (!in_range) {
    double speed = entity->GetSpeedMeters();
    int speed_cells =
        static_cast<int>(std::floor(speed / positioning::kCellSizeMeters));

    positioning::GridPos destination =
        movement_resolver_->FindClosestReachableCell(entity, target,
                                                     speed_cells, ally_ids);

    positioning::GridPos current = position_map_->GetAnchor(entity);
    if (destination != current) {
      auto path = movement_resolver_->PlanMove(entity, destination, speed_cells,
                                               ally_ids);
      if (!path.empty()) {
        movement_resolver_->ApplyMove(entity, path.back());
        logger_->Debug("{} moves to ({}, {})", entity->GetName(), path.back().x,
                       path.back().y);
        LogGrid();
      }
    }
  }

  if (position_map_->GetDistanceMeters(entity, target) > weapon_range) {
    logger_->Info("{} cannot reach {} this turn, skipping attack",
                  entity->GetName(), target->GetName());
    return;
  }

  logger_->Info("--- {}'s turn ---", entity->GetName());

  logger_->Debug("Target current HP: {} and temp HP: {}",
                 target->GetCurrentStats().base_stats.hp,
                 target->GetCurrentStats().bonus_stats.temporary_hp);
  logger_->Debug("Source current HP: {} and temp HP: {}",
                 entity->GetCurrentStats().base_stats.hp,
                 entity->GetCurrentStats().bonus_stats.temporary_hp);

  combat::CombatEventContext context;
  context.source = entity;
  context.distance_meters = position_map_->GetDistanceMeters(entity, target);
  combat::EmitCombatEvent(combat::CombatEvent::TurnStart, &context);

  engine_->QueueAttack({entity, target, attack_sequence_index});

  engine_->Flush(this);
}

void Director::QueueAttack(combat::QueuedAttack attack) {
  double range =
      GetMaxAttackRange(attack.attacker, attack.attack_sequence_index);

  if (!attack.target || !attack.target->IsAlive()) {
    if (attack.attacker) {
      auto enemies = encounter_->GetLivingEnemiesOf(attack.attacker);
      entities::IEntity* best = nullptr;
      for (auto* enemy : enemies) {
        if (position_map_->GetDistanceMeters(attack.attacker, enemy) <= range) {
          best = enemy;
          break;
        }
      }
      if (!best) {
        best = SelectTarget(attack.attacker);
      }
      attack.target = best;
    }
  }

  if (!attack.target) {
    logger_->Info("No living target available for queued attack by {}, dropped",
                  attack.attacker ? attack.attacker->GetName() : "unknown");
    return;
  }

  if (position_map_->GetDistanceMeters(attack.attacker, attack.target) >
      range) {
    logger_->Info("Queued attack by {} on {} dropped - out of range",
                  attack.attacker ? attack.attacker->GetName() : "unknown",
                  attack.target->GetName());
    return;
  }

  engine_->QueueAttack(std::move(attack));
}

void Director::NotifyEntityDied(entities::IEntity* entity) {
  encounter_->NotifyDeath(entity);
  position_map_->Remove(entity);
}

entities::IEntity* Director::SelectTarget(entities::IEntity* attacker) const {
  auto enemies = encounter_->GetLivingEnemiesOf(attacker);
  if (enemies.empty()) {
    return nullptr;
  }

  entities::IEntity* best = nullptr;
  int best_dist = std::numeric_limits<int>::max();

  for (auto* enemy : enemies) {
    int dist = position_map_->GetDistanceCells(attacker, enemy);
    if (dist < best_dist) {
      best_dist = dist;
      best = enemy;
    }
  }

  return best;
}

double Director::GetMaxAttackRange(const entities::IEntity* entity,
                                   int attack_index) const {
  const auto& sequence = entity->GetAttackSequence(attack_index);
  double max_range = std::numeric_limits<double>::max();
  for (const auto& attack : sequence.attacks) {
    if (attack.weapon.range_meters < max_range) {
      max_range = attack.weapon.range_meters;
    }
  }
  return max_range;
}

void Director::LogGrid() const {
  size_t w = static_cast<size_t>(position_map_->GetWidth());
  size_t h = static_cast<size_t>(position_map_->GetHeight());

  std::vector<std::string> rows(h, std::string(w, '.'));

  for (const auto& e : encounter_->GetSideA()) {
    if (!e || !position_map_->HasEntity(e.get())) {
      continue;
    }
    positioning::GridPos anchor = position_map_->GetAnchor(e.get());
    int foot = positioning::FootprintCells(position_map_->GetSize(e.get()));
    for (int dx = 0; dx < foot; ++dx) {
      for (int dy = 0; dy < foot; ++dy) {
        size_t cx = static_cast<size_t>(anchor.x + dx);
        size_t cy = static_cast<size_t>(anchor.y + dy);
        if (cx < w && cy < h) {
          rows[cy][cx] = '#';
        }
      }
    }
  }

  for (const auto& e : encounter_->GetSideB()) {
    if (!e || !position_map_->HasEntity(e.get())) {
      continue;
    }
    positioning::GridPos anchor = position_map_->GetAnchor(e.get());
    int foot = positioning::FootprintCells(position_map_->GetSize(e.get()));
    for (int dx = 0; dx < foot; ++dx) {
      for (int dy = 0; dy < foot; ++dy) {
        size_t cx = static_cast<size_t>(anchor.x + dx);
        size_t cy = static_cast<size_t>(anchor.y + dy);
        if (cx < w && cy < h) {
          rows[cy][cx] = '@';
        }
      }
    }
  }

  std::string grid_str = "\n";
  for (size_t y = h; y-- > 0;) {
    grid_str += rows[y] + "\n";
  }
  logger_->Trace("{}", grid_str);
}

}  // namespace engine
}  // namespace ettes

// -----------------------------------------------------------------------------
// | @file      movement_resolver.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/positioning/movement_resolver.h"

#include <climits>
#include <cmath>
#include <queue>
#include <unordered_set>
#include <vector>

#include "internal/entities/i_entity.h"

namespace ettes {
namespace positioning {

namespace {

struct QEntry {
  int f_cost;
  int g_cost;
  int state;
  size_t seq;

  bool operator>(const QEntry& other) const {
    if (f_cost != other.f_cost) {
      return f_cost > other.f_cost;
    }
    if (g_cost != other.g_cost) {
      return g_cost < other.g_cost;
    }
    return seq > other.seq;
  }
};

int EncodeState(int x, int y, int parity, int h, int pc) {
  return (x * h + y) * pc + parity;
}

GridPos DecodeStatePos(int idx, int h, int pc) {
  int raw = idx / pc;
  return GridPos{raw / h, raw % h};
}

int CheapestStateAt(const std::vector<int>& dist, int x, int y, int h, int pc) {
  int best_cost = INT_MAX;
  int best_idx = -1;
  for (int p = 0; p < pc; ++p) {
    int idx = EncodeState(x, y, p, h, pc);
    if (dist[static_cast<size_t>(idx)] < best_cost) {
      best_cost = dist[static_cast<size_t>(idx)];
      best_idx = idx;
    }
  }
  return best_idx;
}

int FootprintDist(GridPos a_anchor, EntitySize a_size, GridPos b_anchor,
                  EntitySize b_size, DiagonalMode mode) {
  int a_foot = FootprintCells(a_size);
  int b_foot = FootprintCells(b_size);
  int a_max_x = a_anchor.x + a_foot - 1;
  int a_max_y = a_anchor.y + a_foot - 1;
  int b_max_x = b_anchor.x + b_foot - 1;
  int b_max_y = b_anchor.y + b_foot - 1;

  int ddx = 0;
  if (a_max_x < b_anchor.x) {
    ddx = b_anchor.x - a_max_x;
  } else if (b_max_x < a_anchor.x) {
    ddx = a_anchor.x - b_max_x;
  }

  int ddy = 0;
  if (a_max_y < b_anchor.y) {
    ddy = b_anchor.y - a_max_y;
  } else if (b_max_y < a_anchor.y) {
    ddy = a_anchor.y - b_max_y;
  }

  return DiagonalDistance({0, 0}, {ddx, ddy}, mode);
}

constexpr int kDx[] = {1, -1, 0, 0, 1, 1, -1, -1};
constexpr int kDy[] = {0, 0, 1, -1, 1, -1, 1, -1};

}  // namespace

MovementResolver::MovementResolver(PositionMap* position_map)
    : position_map_(position_map) {
}

MovementResolver::SearchResult MovementResolver::BudgetSearch(
    GridPos start, EntitySize size, const entities::IEntity* ignore, int budget,
    GridPos hint, const std::vector<uint32_t>& ally_ids,
    GridPos track_toward) const {
  std::unordered_set<uint32_t> ally_set(ally_ids.begin(), ally_ids.end());
  int w = position_map_->GetWidth();
  int h = position_map_->GetHeight();
  DiagonalMode mode = position_map_->GetDiagonalMode();
  int pc = (mode == DiagonalMode::Alternating) ? 2 : 1;
  size_t total = static_cast<size_t>(w * h * pc);
  bool use_heuristic = (hint.x >= 0 && hint.y >= 0);

  SearchResult sr;
  sr.dist.assign(total, INT_MAX);
  sr.from.assign(total, -1);
  sr.width = w;
  sr.height = h;
  sr.parity_count = pc;

  int start_idx = EncodeState(start.x, start.y, 0, h, pc);
  sr.dist[static_cast<size_t>(start_idx)] = 0;

  size_t seq = 0;
  int start_h = use_heuristic ? DiagonalDistance(start, hint, mode) : 0;

  GridPos effective_track = use_heuristic ? hint : track_toward;
  bool use_track = (effective_track.x >= 0 && effective_track.y >= 0);
  int best_hint_h = INT_MAX;

  std::priority_queue<QEntry, std::vector<QEntry>, std::greater<QEntry>> pq;
  pq.push({start_h, 0, start_idx, seq++});

  while (!pq.empty()) {
    QEntry curr = pq.top();
    pq.pop();

    if (curr.g_cost > sr.dist[static_cast<size_t>(curr.state)]) {
      continue;
    }

    GridPos pos = DecodeStatePos(curr.state, h, pc);

    if (use_track && pos != start &&
        position_map_->IsAreaFree(pos, size, ignore)) {
      int sh = DiagonalDistance(pos, effective_track, mode);
      if (sh < best_hint_h ||
          (sh == best_hint_h &&
           curr.g_cost < sr.dist[static_cast<size_t>(sr.best_toward_hint)])) {
        best_hint_h = sh;
        sr.best_toward_hint = curr.state;
      }
    }

    if (use_heuristic && pos == hint) {
      break;
    }

    int parity = curr.state % pc;

    for (int i = 0; i < 8; ++i) {
      int nx = pos.x + kDx[i];
      int ny = pos.y + kDy[i];

      if (nx < 0 || nx >= w || ny < 0 || ny >= h) {
        continue;
      }

      if (!position_map_->IsAreaFree({nx, ny}, size, ignore)) {
        if (ally_set.empty()) {
          continue;
        }
        const auto* blocker =
            position_map_->GetEntityBlockingArea({nx, ny}, size, ignore);
        if (!blocker || ally_set.find(blocker->GetId()) == ally_set.end()) {
          continue;
        }
      }

      bool is_diag = (kDx[i] != 0 && kDy[i] != 0);
      int step = 1;
      int new_par = parity;

      if (is_diag) {
        switch (mode) {
          case DiagonalMode::Simple:
            break;
          case DiagonalMode::Alternating:
            step = (parity == 0) ? 1 : 2;
            new_par = 1 - parity;
            break;
          case DiagonalMode::Full:
            step = 2;
            break;
        }
      }

      int new_g = curr.g_cost + step;
      if (new_g > budget) {
        continue;
      }

      int nidx = EncodeState(nx, ny, new_par, h, pc);
      if (new_g < sr.dist[static_cast<size_t>(nidx)]) {
        sr.dist[static_cast<size_t>(nidx)] = new_g;
        sr.from[static_cast<size_t>(nidx)] = curr.state;
        int nh = use_heuristic ? DiagonalDistance({nx, ny}, hint, mode) : 0;
        pq.push({new_g + nh, new_g, nidx, seq++});
      }
    }
  }

  return sr;
}

std::vector<GridPos> MovementResolver::ReconstructPath(const SearchResult& sr,
                                                       GridPos start,
                                                       int end_state) const {
  int h = sr.height;
  int pc = sr.parity_count;
  int start_0 = EncodeState(start.x, start.y, 0, h, pc);
  int start_1 = (pc > 1) ? EncodeState(start.x, start.y, 1, h, pc) : -1;

  std::vector<GridPos> reversed;
  int current = end_state;
  while (current != start_0 && current != start_1) {
    reversed.push_back(DecodeStatePos(current, h, pc));
    if (sr.from[static_cast<size_t>(current)] == -1) {
      break;
    }
    current = sr.from[static_cast<size_t>(current)];
  }

  std::vector<GridPos> path;
  path.reserve(reversed.size());
  for (size_t i = reversed.size(); i-- > 0;) {
    path.push_back(reversed[i]);
  }
  return path;
}

std::vector<GridPos> MovementResolver::PlanMove(
    const entities::IEntity* entity, GridPos target, int remaining_speed_cells,
    const std::vector<uint32_t>& ally_ids) const {
  std::vector<GridPos> path;
  if (!entity || !position_map_->HasEntity(entity)) {
    return path;
  }

  GridPos start = position_map_->GetAnchor(entity);
  if (start == target) {
    return path;
  }

  EntitySize size = position_map_->GetSize(entity);

  SearchResult sr = BudgetSearch(start, size, entity, remaining_speed_cells,
                                 target, ally_ids);

  int target_state =
      CheapestStateAt(sr.dist, target.x, target.y, sr.height, sr.parity_count);
  if (target_state != -1 &&
      sr.dist[static_cast<size_t>(target_state)] != INT_MAX) {
    return ReconstructPath(sr, start, target_state);
  }

  if (sr.best_toward_hint != -1) {
    return ReconstructPath(sr, start, sr.best_toward_hint);
  }

  return path;
}

void MovementResolver::ApplyMove(const entities::IEntity* entity,
                                 GridPos target) {
  position_map_->Move(entity, target);
}

GridPos MovementResolver::FindClosestReachableCell(
    const entities::IEntity* entity, const entities::IEntity* target,
    int remaining_speed_cells, const std::vector<uint32_t>& ally_ids) const {
  if (!entity || !target) {
    return {0, 0};
  }
  if (!position_map_->HasEntity(entity) || !position_map_->HasEntity(target)) {
    return position_map_->GetAnchor(entity);
  }

  GridPos target_anchor = position_map_->GetAnchor(target);
  EntitySize target_size = position_map_->GetSize(target);
  int target_foot = FootprintCells(target_size);

  EntitySize entity_size = position_map_->GetSize(entity);
  GridPos entity_anchor = position_map_->GetAnchor(entity);
  DiagonalMode mode = position_map_->GetDiagonalMode();

  SearchResult sr =
      BudgetSearch(entity_anchor, entity_size, entity, remaining_speed_cells,
                   {-1, -1}, ally_ids, target_anchor);

  GridPos best = entity_anchor;
  int best_dist = FootprintDist(entity_anchor, entity_size, target_anchor,
                                target_size, mode);
  int best_travel = 0;

  for (int tx = target_anchor.x - 1; tx <= target_anchor.x + target_foot;
       ++tx) {
    for (int ty = target_anchor.y - 1; ty <= target_anchor.y + target_foot;
         ++ty) {
      GridPos candidate{tx, ty};
      if (candidate.x < 0 || candidate.x >= position_map_->GetWidth() ||
          candidate.y < 0 || candidate.y >= position_map_->GetHeight()) {
        continue;
      }

      int state = CheapestStateAt(sr.dist, candidate.x, candidate.y, sr.height,
                                  sr.parity_count);
      if (state == -1 || sr.dist[static_cast<size_t>(state)] == INT_MAX) {
        continue;
      }

      if (!position_map_->IsAreaFree(candidate, entity_size, entity)) {
        continue;
      }

      int dist = FootprintDist(candidate, entity_size, target_anchor,
                               target_size, mode);
      int travel = DiagonalDistance(entity_anchor, candidate, mode);
      int deviation = std::abs(candidate.x - entity_anchor.x) +
                      std::abs(candidate.y - entity_anchor.y);
      int best_deviation = std::abs(best.x - entity_anchor.x) +
                           std::abs(best.y - entity_anchor.y);

      bool is_better = (dist < best_dist) ||
                       (dist == best_dist && travel < best_travel) ||
                       (dist == best_dist && travel == best_travel &&
                        deviation < best_deviation);
      if (is_better) {
        best_dist = dist;
        best = candidate;
        best_travel = travel;
      }
    }
  }

  if (best == entity_anchor && best_dist > 0 && sr.best_toward_hint != -1) {
    GridPos candidate =
        DecodeStatePos(sr.best_toward_hint, sr.height, sr.parity_count);
    int dist =
        FootprintDist(candidate, entity_size, target_anchor, target_size, mode);
    int travel = DiagonalDistance(entity_anchor, candidate, mode);
    if (dist < best_dist || (dist == best_dist && travel < best_travel)) {
      best_dist = dist;
      best = candidate;
      best_travel = travel;
    }
  }

  return best;
}

}  // namespace positioning
}  // namespace ettes

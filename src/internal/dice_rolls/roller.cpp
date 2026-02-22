// -----------------------------------------------------------------------------
// | @file      roller.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/dice_rolls/roller.h"

#include "internal/dice_rolls/random_engine.h"

namespace ettes {
namespace dice_rolls {

Roller::Roller() : Roller(std::make_shared<StdMt19937Engine>()) {
}

Roller::Roller(std::shared_ptr<IRandomEngine> engine)
    : engine_(std::move(engine)) {
}

Roller::~Roller() = default;

int Roller::Roll(const Term& term) {
  int total = 0;
  for (const auto& dice : term.dice_groups) {
    for (int i = 0; i < dice.count; ++i) {
      total += engine_->GetRand(dice.sides);
    }
  }

  return total + term.bonus;
}

}  // namespace dice_rolls
}  // namespace ettes

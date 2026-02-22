// -----------------------------------------------------------------------------
// | @file      mock_random_engine.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef TESTS_MOCKS_MOCK_RANDOM_ENGINE_H_
#define TESTS_MOCKS_MOCK_RANDOM_ENGINE_H_

#include <gmock/gmock.h>

#include "src/internal/dice_rolls/random_engine.h"

namespace ettes {
namespace dice_rolls {

class MockRandomEngine : public IRandomEngine {
 public:
  MockRandomEngine() = default;
  MOCK_METHOD(int, GetRand, (int), (override));
};

}  // namespace dice_rolls
}  // namespace ettes

#endif  // TESTS_MOCKS_MOCK_RANDOM_ENGINE_H_

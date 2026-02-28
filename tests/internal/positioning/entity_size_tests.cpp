// -----------------------------------------------------------------------------
// | @file      entity_size_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "internal/positioning/entity_size.h"

namespace ettes {
namespace positioning {

TEST(EntitySizeTest, FootprintCellsTiny) {
  EXPECT_EQ(FootprintCells(EntitySize::Tiny), 1);
}

TEST(EntitySizeTest, FootprintCellsSmall) {
  EXPECT_EQ(FootprintCells(EntitySize::Small), 1);
}

TEST(EntitySizeTest, FootprintCellsMedium) {
  EXPECT_EQ(FootprintCells(EntitySize::Medium), 1);
}

TEST(EntitySizeTest, FootprintCellsLarge) {
  EXPECT_EQ(FootprintCells(EntitySize::Large), 2);
}

TEST(EntitySizeTest, FootprintCellsHuge) {
  EXPECT_EQ(FootprintCells(EntitySize::Huge), 3);
}

TEST(EntitySizeTest, FootprintCellsGargantuan) {
  EXPECT_EQ(FootprintCells(EntitySize::Gargantuan), 4);
}

}  // namespace positioning
}  // namespace ettes

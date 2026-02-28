// -----------------------------------------------------------------------------
// | @file      roller_distribution_tests.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>

#include "internal/dice_rolls/roller.h"
#include "internal/dice_rolls/term.h"

namespace ettes {
namespace dice_rolls {

namespace {

constexpr int kRolls = 1'000'000;
constexpr int kBarWidth = 50;

void PrintDistribution(const std::string& label,
                       const std::map<int, int>& dist,
                       int sides) {
  const int max_count =
      std::max_element(dist.begin(), dist.end(),
                       [](const auto& a, const auto& b) {
                         return a.second < b.second;
                       })
          ->second;

  std::cout << "\n  " << label << " distribution (" << kRolls << " rolls)\n";
  std::cout << "  " << std::string(kBarWidth + 22, '-') << "\n";

  for (const auto& [face, count] : dist) {
    const double pct = 100.0 * static_cast<double>(count) /
                       static_cast<double>(kRolls);
    const int bar_len =
        static_cast<int>(kBarWidth * static_cast<double>(count) /
                         static_cast<double>(max_count));

    std::cout << "  " << std::setw(4) << face << " | "
              << std::string(static_cast<std::size_t>(bar_len), '#')
              << std::string(static_cast<std::size_t>(kBarWidth - bar_len), ' ')
              << " | " << std::setw(8) << count << "  ("
              << std::fixed << std::setprecision(3) << pct << "%)\n";
  }

  const double expected_pct = 100.0 / static_cast<double>(sides);
  std::cout << "\n  Expected per face: " << std::fixed << std::setprecision(3)
            << expected_pct << "%\n";
  std::cout << "  " << std::string(kBarWidth + 22, '-') << "\n";
}

}  // namespace

TEST(RollerDistributionTest, OneDTwentyIsUniform) {
  Roller roller;
  Term term;
  term.dice_groups = {Dice{.count = 1, .sides = 20}};

  std::map<int, int> dist;
  for (int i = 0; i < kRolls; ++i) {
    dist[roller.Roll(term)]++;
  }

  PrintDistribution("1d20", dist, 20);

  EXPECT_EQ(static_cast<int>(dist.size()), 20);

  for (const auto& [face, count] : dist) {
    EXPECT_GE(face, 1);
    EXPECT_LE(face, 20);
    const double pct =
        100.0 * static_cast<double>(count) / static_cast<double>(kRolls);
    EXPECT_NEAR(pct, 5.0, 0.1) << "Face " << face << " is outside 1% tolerance";
  }
}

TEST(RollerDistributionTest, OneDHundredIsUniform) {
  Roller roller;
  Term term;
  term.dice_groups = {Dice{.count = 1, .sides = 100}};

  std::map<int, int> dist;
  for (int i = 0; i < kRolls; ++i) {
    dist[roller.Roll(term)]++;
  }

  PrintDistribution("1d100", dist, 100);

  EXPECT_EQ(static_cast<int>(dist.size()), 100);

  for (const auto& [face, count] : dist) {
    EXPECT_GE(face, 1);
    EXPECT_LE(face, 100);
    const double pct =
        100.0 * static_cast<double>(count) / static_cast<double>(kRolls);
    EXPECT_NEAR(pct, 1.0, 0.05) << "Face " << face << " is outside tolerance";
  }
}

}  // namespace dice_rolls
}  // namespace ettes

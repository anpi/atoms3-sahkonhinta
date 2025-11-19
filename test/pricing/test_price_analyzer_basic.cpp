#include <gtest/gtest.h>
#include <vector>
#include <cstdio>

// Use test String adapter before including production headers
#include "../TestStringAdapter.h"
#define WString_h  // Prevent Arduino WString.h inclusion

// Now include production headers and implementation
#include "../../src/pricing/PriceData.h"
#include "../../src/pricing/PriceAnalyzer.cpp"

// Test Suite: calculate90MinAverage
TEST(Calculate90MinAverage, ValidInput_Exactly6Periods) {
  std::vector<PriceEntry> prices = {
    {"2025-11-15T10:00:00", 0.10f},
    {"2025-11-15T10:15:00", 0.11f},
    {"2025-11-15T10:30:00", 0.12f},
    {"2025-11-15T10:45:00", 0.13f},
    {"2025-11-15T11:00:00", 0.14f},
    {"2025-11-15T11:15:00", 0.15f}
  };
  
  float avg = PriceAnalyzer::calculate90MinAverage(prices, 0);
  EXPECT_NEAR(avg, 0.125f, 0.0001f);
}

TEST(Calculate90MinAverage, ValidInput_MoreThan6Periods) {
  std::vector<PriceEntry> prices;
  for (int i = 0; i < 20; i++) {
    prices.push_back({"2025-11-15T10:00:00", 0.10f + i * 0.01f});
  }
  
  float avg = PriceAnalyzer::calculate90MinAverage(prices, 0);
  float expected = (0.10f + 0.11f + 0.12f + 0.13f + 0.14f + 0.15f) / 6.0f;
  EXPECT_NEAR(avg, expected, 0.0001f);
}

TEST(Calculate90MinAverage, NotEnoughPeriodsRemaining) {
  std::vector<PriceEntry> prices;
  for (int i = 0; i < 6; i++) {
    prices.push_back({"2025-11-15T10:00:00", 0.10f});
  }
  
  float avg = PriceAnalyzer::calculate90MinAverage(prices, 1);
  EXPECT_EQ(avg, -1.0f);
}

TEST(Calculate90MinAverage, EmptyDataset) {
  std::vector<PriceEntry> empty;
  float avg = PriceAnalyzer::calculate90MinAverage(empty, 0);
  EXPECT_EQ(avg, -1.0f);
}

// Test Suite: findCheapest90MinPeriod
TEST(FindCheapest90MinPeriod, MultipleWindows_ReturnsLowest) {
  std::vector<PriceEntry> prices = {
    {"2025-11-15T10:00:00", 0.10f},
    {"2025-11-15T10:15:00", 0.12f},
    {"2025-11-15T10:30:00", 0.03f},  // Cheapest period starts here
    {"2025-11-15T10:45:00", 0.04f},
    {"2025-11-15T11:00:00", 0.05f},
    {"2025-11-15T11:15:00", 0.04f},
    {"2025-11-15T11:30:00", 0.03f},
    {"2025-11-15T11:45:00", 0.05f},
    {"2025-11-15T12:00:00", 0.10f}
  };
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  EXPECT_NEAR(result.avg, 0.04f, 0.0001f);
  EXPECT_EQ(result.startIndex, 2);
}

TEST(FindCheapest90MinPeriod, SingleValidWindow) {
  std::vector<PriceEntry> prices;
  for (int i = 0; i < 6; i++) {
    prices.push_back({"2025-11-15T10:00:00", 0.10f + i * 0.01f});
  }
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  EXPECT_NEAR(result.avg, 0.125f, 0.0001f);
  EXPECT_EQ(result.startIndex, 0);
}

TEST(FindCheapest90MinPeriod, AllPricesEqual_ReturnsFirst) {
  std::vector<PriceEntry> prices;
  for (int i = 0; i < 10; i++) {
    prices.push_back({"2025-11-15T10:00:00", 0.10f});
  }
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  EXPECT_NEAR(result.avg, 0.10f, 0.0001f);
  EXPECT_EQ(result.startIndex, 0);
}

TEST(FindCheapest90MinPeriod, FewerThan6Periods_ReturnsInvalid) {
  std::vector<PriceEntry> prices = {
    {"2025-11-15T10:00:00", 0.10f},
    {"2025-11-15T10:15:00", 0.11f},
    {"2025-11-15T10:30:00", 0.12f}
  };
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  EXPECT_EQ(result.avg, -1.0f);
  EXPECT_EQ(result.startIndex, -1);
}

TEST(FindCheapest90MinPeriod, EmptyDataset) {
  std::vector<PriceEntry> empty;
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(empty);
  
  EXPECT_EQ(result.avg, -1.0f);
  EXPECT_EQ(result.startIndex, -1);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

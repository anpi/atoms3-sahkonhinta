#include <gtest/gtest.h>
#include <vector>
#include <cstdio>

// Mock Arduino String before including PriceData
#include "mocks/MockString.h"
#define WString_h  // Prevent Arduino WString.h inclusion

#include "../src/PriceData.h"
#include "test_data_helpers.h"

// We need to create a testable version of the analyzer functions
// that doesn't depend on time() for these specific tests

namespace {

// Helper function to calculate 90min average (copied from PriceAnalyzer)
float calculate90MinAverage(const std::vector<PriceEntry>& prices, int startIdx) {
  const int periods = 6; // 6 * 15min = 90min
  
  if (startIdx + periods > (int)prices.size()) {
    return -1; // Not enough data
  }
  
  float sum = 0;
  for (int i = 0; i < periods; i++) {
    sum += prices[startIdx + i].priceWithTax;
  }
  
  return sum / periods;
}

// Helper function to find cheapest period (copied from PriceAnalyzer)
Cheapest90Min findCheapest90MinPeriod(const std::vector<PriceEntry>& prices) {
  const int periods = 6; // 6 * 15min = 90min
  Cheapest90Min result;
  
  if (prices.size() < (size_t)periods) {
    return result; // Not enough data
  }
  
  float cheapestAvg = 999999.0f;
  String cheapestTime = "";
  
  // Sliding window through all available prices
  for (size_t i = 0; i <= prices.size() - periods; i++) {
    float avg = calculate90MinAverage(prices, i);
    if (avg >= 0 && avg < cheapestAvg) {
      cheapestAvg = avg;
      cheapestTime = prices[i].dateTime.substring(11, 16); // Extract HH:MM
    }
  }
  
  if (cheapestAvg != 999999.0f) {
    result.avg = cheapestAvg;
    result.time = cheapestTime;
  }
  
  return result;
}

} // anonymous namespace

// Test Suite: calculate90MinAverage
class Calculate90MinAverageTest : public ::testing::Test {
protected:
  std::vector<PriceEntry> prices;
  
  void SetUp() override {
    prices = TestHelpers::createMinimalDataset();
  }
};

TEST_F(Calculate90MinAverageTest, ValidInputExactly6Periods) {
  float avg = calculate90MinAverage(prices, 0);
  
  // Expected: (0.10 + 0.11 + 0.12 + 0.13 + 0.14 + 0.15) / 6 = 0.125
  EXPECT_NEAR(avg, 0.125f, 0.0001f);
}

TEST_F(Calculate90MinAverageTest, ValidInputWithMoreThan6Periods) {
  prices.push_back(TestHelpers::createPriceEntry("2025-11-15T11:30:00", 0.20f, 6));
  prices.push_back(TestHelpers::createPriceEntry("2025-11-15T11:45:00", 0.25f, 7));
  
  float avg = calculate90MinAverage(prices, 0);
  
  // Should still use only first 6 periods
  EXPECT_NEAR(avg, 0.125f, 0.0001f);
}

TEST_F(Calculate90MinAverageTest, StartIndexInMiddle) {
  prices = TestHelpers::createStandardDataset();
  
  // Start at index 10, should calculate average of indices 10-15
  float avg = calculate90MinAverage(prices, 10);
  
  EXPECT_GT(avg, 0.0f);
  EXPECT_LT(avg, 1.0f); // Sanity check
}

TEST_F(Calculate90MinAverageTest, NotEnoughPeriodsRemaining) {
  float avg = calculate90MinAverage(prices, 1); // Only 5 periods remaining
  
  EXPECT_EQ(avg, -1.0f);
}

TEST_F(Calculate90MinAverageTest, StartIndexAtEnd) {
  float avg = calculate90MinAverage(prices, prices.size());
  
  EXPECT_EQ(avg, -1.0f);
}

TEST_F(Calculate90MinAverageTest, BoundaryCase_Exactly6PeriodsRemaining) {
  float avg = calculate90MinAverage(prices, 0); // 6 periods total
  
  EXPECT_GT(avg, 0.0f);
}

TEST_F(Calculate90MinAverageTest, EmptyDataset) {
  std::vector<PriceEntry> empty;
  float avg = calculate90MinAverage(empty, 0);
  
  EXPECT_EQ(avg, -1.0f);
}

// Test Suite: findCheapest90MinPeriod
class FindCheapest90MinPeriodTest : public ::testing::Test {
protected:
  std::vector<PriceEntry> prices;
};

TEST_F(FindCheapest90MinPeriodTest, MultipleValidWindows_ReturnsLowest) {
  prices = TestHelpers::createDatasetWithCheapestPeriod();
  
  Cheapest90Min result = findCheapest90MinPeriod(prices);
  
  // Expected cheapest: 10:30 with average of (0.03+0.04+0.05+0.04+0.03+0.05)/6 = 0.04
  EXPECT_NEAR(result.avg, 0.04f, 0.0001f);
  EXPECT_EQ(result.time, String("10:30"));
}

TEST_F(FindCheapest90MinPeriodTest, SingleValidWindow) {
  prices = TestHelpers::createMinimalDataset();
  
  Cheapest90Min result = findCheapest90MinPeriod(prices);
  
  // Only one window possible
  EXPECT_NEAR(result.avg, 0.125f, 0.0001f);
  EXPECT_EQ(result.time, String("10:00"));
}

TEST_F(FindCheapest90MinPeriodTest, AllPricesEqual_ReturnsFirst) {
  for (int i = 0; i < 10; i++) {
    char dateTime[20];
    snprintf(dateTime, sizeof(dateTime), "2025-11-15T10:%02d:00", i * 15);
    prices.push_back(TestHelpers::createPriceEntry(dateTime, 0.10f, i));
  }
  
  Cheapest90Min result = findCheapest90MinPeriod(prices);
  
  EXPECT_NEAR(result.avg, 0.10f, 0.0001f);
  EXPECT_EQ(result.time, String("10:00"));
}

TEST_F(FindCheapest90MinPeriodTest, FewerThan6Periods_ReturnsInvalid) {
  prices.push_back(TestHelpers::createPriceEntry("2025-11-15T10:00:00", 0.10f, 0));
  prices.push_back(TestHelpers::createPriceEntry("2025-11-15T10:15:00", 0.11f, 1));
  prices.push_back(TestHelpers::createPriceEntry("2025-11-15T10:30:00", 0.12f, 2));
  
  Cheapest90Min result = findCheapest90MinPeriod(prices);
  
  EXPECT_EQ(result.avg, -1.0f);
  EXPECT_EQ(result.time, String(""));
}

TEST_F(FindCheapest90MinPeriodTest, TimeExtractionCorrectFormat) {
  prices = TestHelpers::createStandardDataset();
  
  Cheapest90Min result = findCheapest90MinPeriod(prices);
  
  // Time should be in HH:MM format (5 characters)
  EXPECT_EQ(result.time.length(), 5);
  EXPECT_EQ(result.time.c_str()[2], ':'); // Colon at position 2
}

TEST_F(FindCheapest90MinPeriodTest, EmptyDataset) {
  Cheapest90Min result = findCheapest90MinPeriod(prices);
  
  EXPECT_EQ(result.avg, -1.0f);
  EXPECT_EQ(result.time, String(""));
}

// Main function
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

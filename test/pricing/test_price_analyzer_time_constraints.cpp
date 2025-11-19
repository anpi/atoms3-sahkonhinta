#include <gtest/gtest.h>
#include <vector>
#include <cstdio>

// Use test String adapter before including production headers
#include "../TestStringAdapter.h"
#define WString_h  // Prevent Arduino WString.h inclusion

// Include production headers and implementation
#include "../../src/pricing/PriceData.h"
#include "../../src/pricing/PriceAnalyzer.cpp"

// Test Suite: Time-constrained cheapest period (7:00-23:00)
// Requirement: Washing machine can only run during hours 7:00-23:00
// The cheapest 90-minute period must:
// - Start at or after 7:00
// - End at or before 23:00 (last period starts at 21:30, ends at 23:00)

TEST(TimeConstraints, CheapestPeriodAtNight_ShouldBeIgnored) {
  std::vector<PriceEntry> prices;
  
  // Cheapest period is at night (3:00-4:30) but should be ignored
  prices.push_back({"2025-11-16T03:00:00", 0.01f});  // Very cheap but at night
  prices.push_back({"2025-11-16T03:15:00", 0.01f});
  prices.push_back({"2025-11-16T03:30:00", 0.01f});
  prices.push_back({"2025-11-16T03:45:00", 0.01f});
  prices.push_back({"2025-11-16T04:00:00", 0.01f});
  prices.push_back({"2025-11-16T04:15:00", 0.01f});
  
  // Valid period during the day (10:00-11:30)
  prices.push_back({"2025-11-16T10:00:00", 0.10f});
  prices.push_back({"2025-11-16T10:15:00", 0.10f});
  prices.push_back({"2025-11-16T10:30:00", 0.10f});
  prices.push_back({"2025-11-16T10:45:00", 0.10f});
  prices.push_back({"2025-11-16T11:00:00", 0.10f});
  prices.push_back({"2025-11-16T11:15:00", 0.10f});
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  // Should return the 10:00 period, not the 3:00 period
  EXPECT_EQ(result.startIndex, 6);  // Index of 10:00
  EXPECT_NEAR(result.avg, 0.10f, 0.001f);
}

TEST(TimeConstraints, CheapestPeriodStartsAt0700_ShouldBeValid) {
  std::vector<PriceEntry> prices;
  
  // Period starting exactly at 7:00 should be valid
  prices.push_back({"2025-11-16T07:00:00", 0.05f});
  prices.push_back({"2025-11-16T07:15:00", 0.05f});
  prices.push_back({"2025-11-16T07:30:00", 0.05f});
  prices.push_back({"2025-11-16T07:45:00", 0.05f});
  prices.push_back({"2025-11-16T08:00:00", 0.05f});
  prices.push_back({"2025-11-16T08:15:00", 0.05f});
  
  // More expensive period later
  prices.push_back({"2025-11-16T10:00:00", 0.10f});
  prices.push_back({"2025-11-16T10:15:00", 0.10f});
  prices.push_back({"2025-11-16T10:30:00", 0.10f});
  prices.push_back({"2025-11-16T10:45:00", 0.10f});
  prices.push_back({"2025-11-16T11:00:00", 0.10f});
  prices.push_back({"2025-11-16T11:15:00", 0.10f});
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  // Should return the 7:00 period
  EXPECT_EQ(result.startIndex, 0);
  EXPECT_NEAR(result.avg, 0.05f, 0.001f);
}

TEST(TimeConstraints, CheapestPeriodEndsAt2300_ShouldBeValid) {
  std::vector<PriceEntry> prices;
  
  // Period starting at 21:30, ending at 23:00 should be valid
  prices.push_back({"2025-11-16T21:30:00", 0.05f});
  prices.push_back({"2025-11-16T21:45:00", 0.05f});
  prices.push_back({"2025-11-16T22:00:00", 0.05f});
  prices.push_back({"2025-11-16T22:15:00", 0.05f});
  prices.push_back({"2025-11-16T22:30:00", 0.05f});
  prices.push_back({"2025-11-16T22:45:00", 0.05f});  // Ends at 23:00
  
  // More expensive period earlier
  prices.push_back({"2025-11-16T10:00:00", 0.10f});
  prices.push_back({"2025-11-16T10:15:00", 0.10f});
  prices.push_back({"2025-11-16T10:30:00", 0.10f});
  prices.push_back({"2025-11-16T10:45:00", 0.10f});
  prices.push_back({"2025-11-16T11:00:00", 0.10f});
  prices.push_back({"2025-11-16T11:15:00", 0.10f});
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  // Should return the 21:30 period
  EXPECT_EQ(result.startIndex, 0);
  EXPECT_NEAR(result.avg, 0.05f, 0.001f);
}

TEST(TimeConstraints, CheapestPeriodStartsAt2145_ShouldBeInvalid) {
  std::vector<PriceEntry> prices;
  
  // Period starting at 21:45 would end at 23:15 (past 23:00) - invalid
  prices.push_back({"2025-11-16T21:45:00", 0.01f});  // Very cheap but ends too late
  prices.push_back({"2025-11-16T22:00:00", 0.01f});
  prices.push_back({"2025-11-16T22:15:00", 0.01f});
  prices.push_back({"2025-11-16T22:30:00", 0.01f});
  prices.push_back({"2025-11-16T22:45:00", 0.01f});
  prices.push_back({"2025-11-16T23:00:00", 0.01f});
  prices.push_back({"2025-11-16T23:15:00", 0.01f});  // Would end at 23:30 - invalid!
  
  // More expensive but valid period during the day
  prices.push_back({"2025-11-16T10:00:00", 0.10f});
  prices.push_back({"2025-11-16T10:15:00", 0.10f});
  prices.push_back({"2025-11-16T10:30:00", 0.10f});
  prices.push_back({"2025-11-16T10:45:00", 0.10f});
  prices.push_back({"2025-11-16T11:00:00", 0.10f});
  prices.push_back({"2025-11-16T11:15:00", 0.10f});
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  // Should return the 10:00 period, not the 21:45 period
  EXPECT_EQ(result.startIndex, 7);
  EXPECT_NEAR(result.avg, 0.10f, 0.001f);
}

TEST(TimeConstraints, CheapestPeriodStartsAt0645_ShouldBeInvalid) {
  std::vector<PriceEntry> prices;
  
  // Period starting at 6:45 (before 7:00) - invalid
  prices.push_back({"2025-11-16T06:45:00", 0.01f});  // Very cheap but starts too early
  prices.push_back({"2025-11-16T07:00:00", 0.01f});
  prices.push_back({"2025-11-16T07:15:00", 0.01f});
  prices.push_back({"2025-11-16T07:30:00", 0.01f});
  prices.push_back({"2025-11-16T07:45:00", 0.01f});
  prices.push_back({"2025-11-16T08:00:00", 0.01f});
  prices.push_back({"2025-11-16T08:15:00", 0.01f});  // Need this for 7:00 period to complete
  
  // More expensive valid period starting at 10:00
  prices.push_back({"2025-11-16T10:00:00", 0.10f});
  prices.push_back({"2025-11-16T10:15:00", 0.10f});
  prices.push_back({"2025-11-16T10:30:00", 0.10f});
  prices.push_back({"2025-11-16T10:45:00", 0.10f});
  prices.push_back({"2025-11-16T11:00:00", 0.10f});
  prices.push_back({"2025-11-16T11:15:00", 0.10f});
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  // Should return the 7:00 period (valid), not the 6:45 period (invalid)
  EXPECT_EQ(result.startIndex, 1);
  EXPECT_NEAR(result.avg, 0.01f, 0.001f);
}

TEST(TimeConstraints, MultiplePeriodsDuringDay_ReturnsActualCheapest) {
  std::vector<PriceEntry> prices;
  
  // Period 1: 8:00-9:30 (avg 0.12)
  prices.push_back({"2025-11-16T08:00:00", 0.12f});
  prices.push_back({"2025-11-16T08:15:00", 0.12f});
  prices.push_back({"2025-11-16T08:30:00", 0.12f});
  prices.push_back({"2025-11-16T08:45:00", 0.12f});
  prices.push_back({"2025-11-16T09:00:00", 0.12f});
  prices.push_back({"2025-11-16T09:15:00", 0.12f});
  
  // Period 2: 14:00-15:30 (avg 0.08) - cheapest valid
  prices.push_back({"2025-11-16T14:00:00", 0.08f});
  prices.push_back({"2025-11-16T14:15:00", 0.08f});
  prices.push_back({"2025-11-16T14:30:00", 0.08f});
  prices.push_back({"2025-11-16T14:45:00", 0.08f});
  prices.push_back({"2025-11-16T15:00:00", 0.08f});
  prices.push_back({"2025-11-16T15:15:00", 0.08f});
  
  // Period 3: 20:00-21:30 (avg 0.10)
  prices.push_back({"2025-11-16T20:00:00", 0.10f});
  prices.push_back({"2025-11-16T20:15:00", 0.10f});
  prices.push_back({"2025-11-16T20:30:00", 0.10f});
  prices.push_back({"2025-11-16T20:45:00", 0.10f});
  prices.push_back({"2025-11-16T21:00:00", 0.10f});
  prices.push_back({"2025-11-16T21:15:00", 0.10f});
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  // Should return the 14:00 period
  EXPECT_EQ(result.startIndex, 6);
  EXPECT_NEAR(result.avg, 0.08f, 0.001f);
}

TEST(TimeConstraints, AllPeriodsOutsideValidHours_ReturnsInvalid) {
  std::vector<PriceEntry> prices;
  
  // Only night periods available
  prices.push_back({"2025-11-16T23:00:00", 0.05f});
  prices.push_back({"2025-11-16T23:15:00", 0.05f});
  prices.push_back({"2025-11-16T23:30:00", 0.05f});
  prices.push_back({"2025-11-16T23:45:00", 0.05f});
  prices.push_back({"2025-11-17T00:00:00", 0.05f});
  prices.push_back({"2025-11-17T00:15:00", 0.05f});
  prices.push_back({"2025-11-17T02:00:00", 0.05f});
  prices.push_back({"2025-11-17T02:15:00", 0.05f});
  prices.push_back({"2025-11-17T02:30:00", 0.05f});
  prices.push_back({"2025-11-17T02:45:00", 0.05f});
  prices.push_back({"2025-11-17T03:00:00", 0.05f});
  prices.push_back({"2025-11-17T03:15:00", 0.05f});
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  // Should return invalid result
  EXPECT_EQ(result.startIndex, -1);
  EXPECT_EQ(result.avg, -1.0f);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>
#include <vector>
#include <cstdio>

// Use test String adapter before including production headers
#include "../TestStringAdapter.h"
#define WString_h  // Prevent Arduino WString.h inclusion

// Include production headers and implementation
#include "../../src/pricing/PriceData.h"
#include "../../src/pricing/PriceAnalyzer.cpp"

// BUG: What if current time is near the end of available data?
// User at 23:00, only data until 23:45 - not enough for 90min average
TEST(RealisticBugs, CurrentTimeNearEndOfData_NotEnoughFuture) {
  std::vector<PriceEntry> prices;
  
  // Simulate we're at 23:00 and only have data until 23:45 (4 periods)
  prices.push_back({"2025-11-15T23:00:00", 0.10f});
  prices.push_back({"2025-11-15T23:15:00", 0.11f});
  prices.push_back({"2025-11-15T23:30:00", 0.12f});
  prices.push_back({"2025-11-15T23:45:00", 0.13f});
  
  // Try to calculate 90min average from index 0 - needs 6 periods but only have 4
  float avg = PriceAnalyzer::calculate90MinAverage(prices, 0);
  
  std::cout << "Average with insufficient data: " << avg << std::endl;
  
  // Result should be -1 (not enough data)
  EXPECT_EQ(avg, -1.0f);
  
  // The display should show "N/A" for next 90min average
  // This is CORRECT behavior, just documenting it
}

// BUG: DateTime format variations from API
TEST(RealisticBugs, DateTimeFormat_WithMilliseconds) {
  PriceEntry entry;
  entry.dateTime = String("2025-11-15T14:30:00.000+02:00");  // With milliseconds
  entry.priceWithTax = 0.10f;
  
  // Extract HH:MM (positions 11-16)
  String time = entry.dateTime.substring(11, 16);
  
  std::cout << "Extracted time: '" << time.c_str() << "'" << std::endl;
  
  // Will extract "14:30" correctly even with milliseconds
  EXPECT_EQ(time, String("14:30"));
}

// BUG: What happens with duplicate timestamps in data?
TEST(RealisticBugs, DuplicateTimestamps) {
  std::vector<PriceEntry> prices;
  
  // Same timestamp twice (API error or DST transition)
  prices.push_back({"2025-11-15T10:00:00", 0.10f});
  prices.push_back({"2025-11-15T10:00:00", 0.15f});  // Duplicate!
  prices.push_back({"2025-11-15T10:15:00", 0.11f});
  prices.push_back({"2025-11-15T10:30:00", 0.12f});
  prices.push_back({"2025-11-15T10:45:00", 0.13f});
  prices.push_back({"2025-11-15T11:00:00", 0.14f});
  prices.push_back({"2025-11-15T11:15:00", 0.15f});
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  std::cout << "Cheapest with duplicates: " << result.avg << " @ index " << result.startIndex << std::endl;
  
  // Function will process it normally, just taking first 6 entries
  // Average of [0.10, 0.15, 0.11, 0.12, 0.13, 0.14] = 0.125
  EXPECT_NEAR(result.avg, 0.125f, 0.001f);
  EXPECT_EQ(result.startIndex, 0);
}

// BUG: Non-sorted price data
TEST(RealisticBugs, PricesNotChronologicallySorted) {
  std::vector<PriceEntry> prices;
  
  // Prices in wrong order
  prices.push_back({"2025-11-15T10:30:00", 0.12f});
  prices.push_back({"2025-11-15T10:00:00", 0.10f});  // Out of order!
  prices.push_back({"2025-11-15T10:15:00", 0.11f});
  prices.push_back({"2025-11-15T11:00:00", 0.14f});
  prices.push_back({"2025-11-15T10:45:00", 0.13f});  // Out of order!
  prices.push_back({"2025-11-15T11:15:00", 0.15f});
  
  Cheapest90Min result = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  std::cout << "Cheapest with unsorted: " << result.avg << " @ index " << result.startIndex << std::endl;
  
  // POTENTIAL BUG: The algorithm assumes chronological order!
  // It will calculate windows based on array index, not timestamp
  // First window: [0.12, 0.10, 0.11, 0.14, 0.13, 0.15] = 0.125
  // But this is NOT a valid 90-minute window in real time!
  EXPECT_NEAR(result.avg, 0.125f, 0.001f);
  EXPECT_EQ(result.startIndex, 0);  // First index, but misleading timestamp!
  
  // This is a REAL BUG if the API ever returns unsorted data
}

// BUG: Missing intermediate timestamps (gaps in data)
TEST(RealisticBugs, MissingTimestamps_GapsInData) {
  std::vector<PriceEntry> prices;
  
  // Missing 10:15 and 10:30 - gap in the data
  prices.push_back({"2025-11-15T10:00:00", 0.10f});
  // Missing 10:15
  // Missing 10:30
  prices.push_back({"2025-11-15T10:45:00", 0.13f});
  prices.push_back({"2025-11-15T11:00:00", 0.14f});
  prices.push_back({"2025-11-15T11:15:00", 0.15f});
  prices.push_back({"2025-11-15T11:30:00", 0.16f});
  prices.push_back({"2025-11-15T11:45:00", 0.17f});
  prices.push_back({"2025-11-15T12:00:00", 0.18f});
  
  float avg = PriceAnalyzer::calculate90MinAverage(prices, 0);
  
  std::cout << "Average with gaps: " << avg << std::endl;
  
  // POTENTIAL BUG: Calculates average of first 6 array entries
  // But these don't represent 90 continuous minutes!
  // Array: [10:00, 10:45, 11:00, 11:15, 11:30, 11:45]
  // This spans 105 minutes, not 90!
  float expected = (0.10f + 0.13f + 0.14f + 0.15f + 0.16f + 0.17f) / 6.0f;
  EXPECT_NEAR(avg, expected, 0.001f);
  
  // This is a REAL BUG if data has gaps - the "90 minute" average is wrong
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

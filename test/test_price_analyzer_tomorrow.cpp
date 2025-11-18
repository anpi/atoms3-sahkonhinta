#include <gtest/gtest.h>
#include <vector>
#include <cstdio>

// Use test String adapter before including production headers
#include "TestStringAdapter.h"
#define WString_h  // Prevent Arduino WString.h inclusion

// Include production headers and implementation
#include "../src/PriceData.h"
#include "../src/PriceAnalyzer.cpp"

// Helper to create timestamp strings
String makeTimestamp(int year, int month, int day, int hour, int minute) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:00",
           year, month, day, hour, minute);
  return String(buf);
}

// Test Suite: Tomorrow detection and date comparison logic

TEST(TomorrowDetection, SameDay_AllEntriesSameDate) {
  std::vector<PriceEntry> prices;
  
  // All entries from 2025-11-17
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 17, hour, minute), 0.10f});
    }
  }
  
  Cheapest90Min cheapest = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  ASSERT_GE(cheapest.startIndex, 0);
  
  // Extract dates from first entry and cheapest entry
  String firstDate = prices[0].dateTime.substring(0, 10);
  String cheapestDate = prices[cheapest.startIndex].dateTime.substring(0, 10);
  
  EXPECT_EQ(firstDate, cheapestDate);
  EXPECT_EQ(firstDate, String("2025-11-17"));
}

TEST(TomorrowDetection, TwoDays_CheapestToday) {
  std::vector<PriceEntry> prices;
  
  // Today (2025-11-17) - cheap during day
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 17, hour, minute), 0.08f});
    }
  }
  
  // Tomorrow (2025-11-18) - expensive
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 18, hour, minute), 0.20f});
    }
  }
  
  Cheapest90Min cheapest = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  ASSERT_GE(cheapest.startIndex, 0);
  
  // Cheapest should be from today
  String cheapestDate = prices[cheapest.startIndex].dateTime.substring(0, 10);
  EXPECT_EQ(cheapestDate, String("2025-11-17"));
}

TEST(TomorrowDetection, TwoDays_CheapestTomorrow) {
  std::vector<PriceEntry> prices;
  
  // Today (2025-11-17) - expensive during day
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 17, hour, minute), 0.20f});
    }
  }
  
  // Tomorrow (2025-11-18) - cheap
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 18, hour, minute), 0.08f});
    }
  }
  
  Cheapest90Min cheapest = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  ASSERT_GE(cheapest.startIndex, 0);
  
  // Cheapest should be from tomorrow
  String cheapestDate = prices[cheapest.startIndex].dateTime.substring(0, 10);
  EXPECT_EQ(cheapestDate, String("2025-11-18"));
}

TEST(TomorrowDetection, MonthBoundary_CheapestNextMonth) {
  std::vector<PriceEntry> prices;
  
  // End of November (2025-11-30) - expensive
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 30, hour, minute), 0.20f});
    }
  }
  
  // Start of December (2025-12-01) - cheap
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 12, 1, hour, minute), 0.08f});
    }
  }
  
  Cheapest90Min cheapest = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  ASSERT_GE(cheapest.startIndex, 0);
  
  // Cheapest should be from December
  String cheapestDate = prices[cheapest.startIndex].dateTime.substring(0, 10);
  EXPECT_EQ(cheapestDate, String("2025-12-01"));
}

TEST(TomorrowDetection, YearBoundary_CheapestNextYear) {
  std::vector<PriceEntry> prices;
  
  // End of year (2025-12-31) - expensive
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 12, 31, hour, minute), 0.20f});
    }
  }
  
  // New year (2026-01-01) - cheap
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2026, 1, 1, hour, minute), 0.08f});
    }
  }
  
  Cheapest90Min cheapest = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  ASSERT_GE(cheapest.startIndex, 0);
  
  // Cheapest should be from 2026
  String cheapestDate = prices[cheapest.startIndex].dateTime.substring(0, 10);
  EXPECT_EQ(cheapestDate, String("2026-01-01"));
}

TEST(TomorrowDetection, ThreeDays_CheapestOnDayThree) {
  std::vector<PriceEntry> prices;
  
  // Day 1 (2025-11-17) - expensive
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 17, hour, minute), 0.20f});
    }
  }
  
  // Day 2 (2025-11-18) - moderate
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 18, hour, minute), 0.15f});
    }
  }
  
  // Day 3 (2025-11-19) - cheap
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 19, hour, minute), 0.05f});
    }
  }
  
  Cheapest90Min cheapest = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  ASSERT_GE(cheapest.startIndex, 0);
  
  // Cheapest should be from day 3
  String cheapestDate = prices[cheapest.startIndex].dateTime.substring(0, 10);
  EXPECT_EQ(cheapestDate, String("2025-11-19"));
}

TEST(TomorrowDetection, DateSubstring_CorrectLength) {
  std::vector<PriceEntry> prices;
  
  // Add some entries
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  
  // Test that substring(0, 10) gives YYYY-MM-DD
  String date = prices[0].dateTime.substring(0, 10);
  
  EXPECT_EQ(date.length(), static_cast<size_t>(10));
  EXPECT_EQ(date.c_str()[4], '-');
  EXPECT_EQ(date.c_str()[7], '-');
}

TEST(TomorrowDetection, TimeSubstring_CorrectLength) {
  std::vector<PriceEntry> prices;
  
  // Add some entries
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 14, 45), 0.11f});
  
  // Test that substring(11, 16) gives HH:MM
  String time1 = prices[0].dateTime.substring(11, 16);
  String time2 = prices[1].dateTime.substring(11, 16);
  
  EXPECT_EQ(time1, String("10:30"));
  EXPECT_EQ(time2, String("14:45"));
  EXPECT_EQ(time1.length(), static_cast<size_t>(5));
  EXPECT_EQ(time2.length(), static_cast<size_t>(5));
}

TEST(TomorrowDetection, MixedDates_CheapestInMiddle) {
  std::vector<PriceEntry> prices;
  
  // Yesterday evening (2025-11-16) - moderate
  for (int hour = 20; hour <= 23; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 16, hour, minute), 0.15f});
    }
  }
  
  // Today early morning (2025-11-17) - expensive (before 7:00, will be ignored)
  for (int hour = 0; hour < 7; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 17, hour, minute), 0.25f});
    }
  }
  
  // Today daytime (2025-11-17) - cheap
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 17, hour, minute), 0.08f});
    }
  }
  
  // Tomorrow (2025-11-18) - expensive
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 18, hour, minute), 0.20f});
    }
  }
  
  Cheapest90Min cheapest = PriceAnalyzer::findCheapest90MinPeriod(prices);
  
  ASSERT_GE(cheapest.startIndex, 0);
  
  // Cheapest should be from today's daytime
  String cheapestDate = prices[cheapest.startIndex].dateTime.substring(0, 10);
  EXPECT_EQ(cheapestDate, String("2025-11-17"));
  
  String cheapestTime = prices[cheapest.startIndex].dateTime.substring(11, 16);
  int hour = (cheapestTime.c_str()[0] - '0') * 10 + (cheapestTime.c_str()[1] - '0');
  EXPECT_GE(hour, 7);  // Should be in valid time range
}

TEST(TomorrowDetection, IdenticalDates_StringComparison) {
  String date1 = String("2025-11-17");
  String date2 = String("2025-11-17");
  String date3 = String("2025-11-18");
  
  EXPECT_EQ(date1, date2);
  EXPECT_NE(date1, date3);
  EXPECT_TRUE(date1 == date2);
  EXPECT_FALSE(date1 == date3);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

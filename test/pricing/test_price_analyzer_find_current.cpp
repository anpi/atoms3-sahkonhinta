#include <gtest/gtest.h>
#include <vector>
#include <cstdio>
#include <ctime>
#include <cstring>

// Use test String adapter before including production headers
#include "../TestStringAdapter.h"
#define WString_h  // Prevent Arduino WString.h inclusion

// Include production headers and implementation
#include "../../src/pricing/PriceData.h"
#include "../../src/pricing/PriceAnalyzer.cpp"

// Helper to create timestamp strings
String makeTimestamp(int year, int month, int day, int hour, int minute) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:00",
           year, month, day, hour, minute);
  return String(buf);
}

// Mock time for testing - these will be used by time() and localtime()
static time_t mock_time_value = 0;
static struct tm mock_tm_value;
static bool use_mock_time = false;

// Override time() for testing
extern "C" time_t time(time_t* arg) {
  if (use_mock_time) {
    if (arg) *arg = mock_time_value;
    return mock_time_value;
  }
  // Fall back to real time if not mocking
  time_t result = std::time(nullptr);
  if (arg) *arg = result;
  return result;
}

// Override localtime() for testing
extern "C" struct tm* localtime(const time_t* timer) {
  if (use_mock_time) {
    return &mock_tm_value;
  }
  // Fall back to real localtime if not mocking
  return std::localtime(timer);
}

// Helper to set mock time
void setMockTime(int year, int month, int day, int hour, int minute) {
  use_mock_time = true;
  mock_tm_value.tm_year = year - 1900;
  mock_tm_value.tm_mon = month - 1;
  mock_tm_value.tm_mday = day;
  mock_tm_value.tm_hour = hour;
  mock_tm_value.tm_min = minute;
  mock_tm_value.tm_sec = 0;
  mock_time_value = 0; // We don't actually need this for our test
}

void disableMockTime() {
  use_mock_time = false;
}

// Test Suite: findCurrentPriceIndex

TEST(FindCurrentPriceIndex, ExactMatch_FirstEntry) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 10:00
  setMockTime(2025, 11, 17, 10, 0);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.12f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, 0);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, ExactMatch_MiddleEntry) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 10:30
  setMockTime(2025, 11, 17, 10, 30);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.12f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 45), 0.13f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, 2);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, ExactMatch_LastEntry) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 10:45
  setMockTime(2025, 11, 17, 10, 45);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.12f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 45), 0.13f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, 3);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, RoundDown_To15MinuteBoundary) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 10:07 (should round down to 10:00)
  setMockTime(2025, 11, 17, 10, 7);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.12f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, 0);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, RoundDown_To30MinuteBoundary) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 10:42 (should round down to 10:30)
  setMockTime(2025, 11, 17, 10, 42);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.12f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 45), 0.13f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, 2);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, RoundDown_To45MinuteBoundary) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 10:59 (should round down to 10:45)
  setMockTime(2025, 11, 17, 10, 59);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.12f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 45), 0.13f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, 3);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, NoMatch_BeforeDataStarts) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 09:00 (before data starts at 10:00)
  setMockTime(2025, 11, 17, 9, 0);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.12f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, -1);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, NoMatch_AfterDataEnds) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 11:00 (after data ends at 10:45)
  setMockTime(2025, 11, 17, 11, 0);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.12f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 45), 0.13f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, -1);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, NoMatch_WrongDay) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-18 10:00 (data is from 2025-11-17)
  setMockTime(2025, 11, 18, 10, 0);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.12f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, -1);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, EmptyDataset_ReturnsNegative) {
  std::vector<PriceEntry> empty;
  
  setMockTime(2025, 11, 17, 10, 0);
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(empty);
  
  EXPECT_EQ(idx, -1);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, MidnightBoundary_00_00) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 00:00
  setMockTime(2025, 11, 17, 0, 0);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 0, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 0, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 0, 30), 0.12f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, 0);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, MidnightBoundary_23_45) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 23:55 (should round down to 23:45)
  setMockTime(2025, 11, 17, 23, 55);
  
  prices.push_back({makeTimestamp(2025, 11, 17, 23, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 23, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 23, 30), 0.12f});
  prices.push_back({makeTimestamp(2025, 11, 17, 23, 45), 0.13f});
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  EXPECT_EQ(idx, 3);
  
  disableMockTime();
}

TEST(FindCurrentPriceIndex, DataSpanningTwoDays_FindsTodayEntry) {
  std::vector<PriceEntry> prices;
  
  // Set mock time to 2025-11-17 14:00
  setMockTime(2025, 11, 17, 14, 0);
  
  // Data from yesterday evening
  prices.push_back({makeTimestamp(2025, 11, 16, 22, 0), 0.08f});
  prices.push_back({makeTimestamp(2025, 11, 16, 22, 15), 0.09f});
  prices.push_back({makeTimestamp(2025, 11, 16, 22, 30), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 16, 22, 45), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 16, 23, 0), 0.12f});
  
  // Today's data
  for (int hour = 0; hour <= 14; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      prices.push_back({makeTimestamp(2025, 11, 17, hour, minute), 0.15f});
    }
  }
  
  int idx = PriceAnalyzer::findCurrentPriceIndex(prices);
  
  // Should find today's 14:00 entry, not yesterday's data
  EXPECT_GE(idx, 5);  // After yesterday's 5 entries
  if (idx >= 0) {
    EXPECT_TRUE(prices[idx].dateTime.startsWith(String("2025-11-17T14:00")));
  }
  
  disableMockTime();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

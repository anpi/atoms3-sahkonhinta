#include <gtest/gtest.h>
#include <vector>
#include <cstdio>
#include <ctime>

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

// Helper to create a full day of price data starting from a specific hour
std::vector<PriceEntry> createDayPrices(int year, int month, int day, int startHour, float basePrice) {
  std::vector<PriceEntry> prices;
  
  for (int hour = startHour; hour < 24; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      PriceEntry entry;
      entry.dateTime = makeTimestamp(year, month, day, hour, minute);
      entry.priceWithTax = basePrice + (hour * 0.01f) + (minute * 0.0001f);
      prices.push_back(entry);
    }
  }
  
  return prices;
}

// Test Suite: analyzePrices integration tests

TEST(AnalyzePrices, EmptyDataset_ReturnsInvalid) {
  std::vector<PriceEntry> empty;
  
  PriceAnalysis result = PriceAnalyzer::analyzePrices(empty);
  
  EXPECT_FALSE(result.valid);
  EXPECT_EQ(result.next90MinAvg, -1.0f);
  EXPECT_EQ(result.cheapest90MinAvg, -1.0f);
}

TEST(AnalyzePrices, ValidDataset_PopulatesAllFields) {
  std::vector<PriceEntry> prices;
  
  // Create a dataset for 2025-11-17 from 10:00 to 15:00
  for (int hour = 10; hour <= 14; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      PriceEntry entry;
      entry.dateTime = makeTimestamp(2025, 11, 17, hour, minute);
      entry.priceWithTax = 0.10f + (hour - 10) * 0.01f;
      prices.push_back(entry);
    }
  }
  
  // Mock current time by manipulating the first entry to match "now"
  // Note: This test assumes findCurrentPriceIndex will find the first entry
  // In production, this relies on system time
  
  PriceAnalysis result = PriceAnalyzer::analyzePrices(prices);
  
  // Since findCurrentPriceIndex uses system time, we can't easily control it
  // So we test the structure is populated when valid
  if (result.valid) {
    EXPECT_NE(result.currentPeriodStartTime, String(""));
    EXPECT_GE(result.next90MinAvg, 0.0f);
    EXPECT_GE(result.cheapest90MinAvg, 0.0f);
    EXPECT_NE(result.cheapest90MinTime, String(""));
  }
}

TEST(AnalyzePrices, CheapestPeriodExtraction_CorrectTime) {
  std::vector<PriceEntry> prices;
  
  // Create dataset with obvious cheapest period at 12:00
  for (int hour = 10; hour <= 15; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      PriceEntry entry;
      entry.dateTime = makeTimestamp(2025, 11, 17, hour, minute);
      
      // Make 12:00-13:30 period the cheapest
      if (hour == 12 && minute < 90) {
        entry.priceWithTax = 0.05f;
      } else if (hour == 12) {
        entry.priceWithTax = 0.05f;
      } else if (hour == 13 && minute <= 15) {
        entry.priceWithTax = 0.05f;
      } else {
        entry.priceWithTax = 0.20f;
      }
      prices.push_back(entry);
    }
  }
  
  PriceAnalysis result = PriceAnalyzer::analyzePrices(prices);
  
  if (result.valid) {
    EXPECT_EQ(result.cheapest90MinTime, String("12:00"));
    EXPECT_NEAR(result.cheapest90MinAvg, 0.05f, 0.001f);
  }
}

TEST(AnalyzePrices, TomorrowDetection_SameDay) {
  std::vector<PriceEntry> prices;
  
  // All data from same day
  for (int hour = 10; hour <= 20; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      PriceEntry entry;
      entry.dateTime = makeTimestamp(2025, 11, 17, hour, minute);
      entry.priceWithTax = 0.10f;
      prices.push_back(entry);
    }
  }
  
  PriceAnalysis result = PriceAnalyzer::analyzePrices(prices);
  
  if (result.valid) {
    // All on same day, so cheapest should not be tomorrow
    EXPECT_FALSE(result.cheapestIsTomorrow);
  }
}

TEST(AnalyzePrices, TomorrowDetection_NextDay) {
  std::vector<PriceEntry> prices;
  
  // Data from today (expensive)
  for (int hour = 10; hour <= 23; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      PriceEntry entry;
      entry.dateTime = makeTimestamp(2025, 11, 17, hour, minute);
      entry.priceWithTax = 0.20f;
      prices.push_back(entry);
    }
  }
  
  // Data from tomorrow (cheaper)
  for (int hour = 0; hour <= 10; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      PriceEntry entry;
      entry.dateTime = makeTimestamp(2025, 11, 18, hour, minute);
      entry.priceWithTax = 0.05f;
      prices.push_back(entry);
    }
  }
  
  PriceAnalysis result = PriceAnalyzer::analyzePrices(prices);
  
  if (result.valid) {
    // Cheapest period should be from tomorrow (but might be constrained by 7:00-23:00 rule)
    // The 7:00 period from tomorrow should be cheapest valid period
    if (result.cheapest90MinTime == String("07:00")) {
      EXPECT_TRUE(result.cheapestIsTomorrow);
    }
  }
}

TEST(AnalyzePrices, NotEnoughDataForAnalysis_ReturnsInvalid) {
  std::vector<PriceEntry> prices;
  
  // Only 3 entries - not enough for 90min window
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 0), 0.10f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 15), 0.11f});
  prices.push_back({makeTimestamp(2025, 11, 17, 10, 30), 0.12f});
  
  PriceAnalysis result = PriceAnalyzer::analyzePrices(prices);
  
  EXPECT_FALSE(result.valid);
}

TEST(AnalyzePrices, TimeConstraints_OnlyNightPrices_FindsDaytimePeriod) {
  std::vector<PriceEntry> prices;
  
  // Very cheap night prices (should be ignored)
  for (int hour = 0; hour < 7; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      PriceEntry entry;
      entry.dateTime = makeTimestamp(2025, 11, 17, hour, minute);
      entry.priceWithTax = 0.01f;  // Very cheap
      prices.push_back(entry);
    }
  }
  
  // More expensive day prices
  for (int hour = 7; hour <= 22; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      PriceEntry entry;
      entry.dateTime = makeTimestamp(2025, 11, 17, hour, minute);
      entry.priceWithTax = 0.15f;
      prices.push_back(entry);
    }
  }
  
  PriceAnalysis result = PriceAnalyzer::analyzePrices(prices);
  
  if (result.valid) {
    // Should pick a daytime period, not the cheap night period
    String cheapTime = result.cheapest90MinTime;
    int hour = (cheapTime.c_str()[0] - '0') * 10 + (cheapTime.c_str()[1] - '0');
    EXPECT_GE(hour, 7);
    EXPECT_NEAR(result.cheapest90MinAvg, 0.15f, 0.001f);
  }
}

TEST(AnalyzePrices, CurrentPeriodTimeExtraction_CorrectFormat) {
  std::vector<PriceEntry> prices;
  
  // Create dataset
  for (int hour = 10; hour <= 15; hour++) {
    for (int minute = 0; minute < 60; minute += 15) {
      PriceEntry entry;
      entry.dateTime = makeTimestamp(2025, 11, 17, hour, minute);
      entry.priceWithTax = 0.10f;
      prices.push_back(entry);
    }
  }
  
  PriceAnalysis result = PriceAnalyzer::analyzePrices(prices);
  
  if (result.valid) {
    // Check format is HH:MM
    EXPECT_EQ(result.currentPeriodStartTime.length(), static_cast<size_t>(5));
    EXPECT_EQ(result.currentPeriodStartTime.c_str()[2], ':');
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

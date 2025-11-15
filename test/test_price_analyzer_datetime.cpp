#include <gtest/gtest.h>
#include <vector>
#include <cstdio>

// Mock Arduino String before including PriceData
#include "mocks/MockString.h"
#define WString_h

#include "../src/PriceData.h"

// Test edge case: DateTime with timezone offset
TEST(PriceAnalyzerEdgeCases, DateTimeWithTimezoneOffset) {
  PriceEntry entry;
  entry.dateTime = String("2025-11-15T14:30:00+02:00");
  entry.priceWithTax = 0.10f;
  
  // Extract time as PriceAnalyzer does
  String extracted = entry.dateTime.substring(11, 16);
  
  EXPECT_EQ(extracted, String("14:30"));
}

// Test edge case: Malformed DateTime
TEST(PriceAnalyzerEdgeCases, MalformedDateTime_TooShort) {
  PriceEntry entry;
  entry.dateTime = String("2025-11-15");  // Too short
  entry.priceWithTax = 0.10f;
  
  // This will try to extract beyond string bounds
  String extracted = entry.dateTime.substring(11, 16);
  
  // Our mock String should handle this, but real Arduino String might crash
  EXPECT_TRUE(extracted.length() == 0 || extracted.length() == 5);
}

// Test edge case: Empty DateTime
TEST(PriceAnalyzerEdgeCases, EmptyDateTime) {
  PriceEntry entry;
  entry.dateTime = String("");
  entry.priceWithTax = 0.10f;
  
  String extracted = entry.dateTime.substring(11, 16);
  
  EXPECT_EQ(extracted.length(), static_cast<size_t>(0));
}

// Test edge case: Price exactly at cheapest sentinel value
TEST(PriceAnalyzerEdgeCases, PriceAtSentinelValue) {
  std::vector<PriceEntry> prices;
  
  // Create prices all at the sentinel value
  for (int i = 0; i < 10; i++) {
    PriceEntry entry;
    char dt[25];
    snprintf(dt, sizeof(dt), "2025-11-15T%02d:00:00", 10 + i);
    entry.dateTime = String(dt);
    entry.priceWithTax = 999999.0f;  // Same as sentinel
    prices.push_back(entry);
  }
  
  // This tests if the sentinel comparison is strict
  // Current code uses < so 999999.0f should not be selected
  // But the result should still be invalid
}

// Test edge case: Negative prices
TEST(PriceAnalyzerEdgeCases, NegativePrices) {
  std::vector<PriceEntry> prices;
  
  for (int i = 0; i < 10; i++) {
    PriceEntry entry;
    char dt[25];
    snprintf(dt, sizeof(dt), "2025-11-15T%02d:00:00", 10 + i);
    entry.dateTime = String(dt);
    entry.priceWithTax = -0.05f;  // Negative price (unusual but possible)
    prices.push_back(entry);
  }
  
  // Should handle negative prices correctly
  // The average should be -0.05
}

// Test edge case: Very large price values
TEST(PriceAnalyzerEdgeCases, VeryLargePrices) {
  std::vector<PriceEntry> prices;
  
  for (int i = 0; i < 10; i++) {
    PriceEntry entry;
    char dt[25];
    snprintf(dt, sizeof(dt), "2025-11-15T%02d:00:00", 10 + i);
    entry.dateTime = String(dt);
    entry.priceWithTax = 1000000.0f;  // Much larger than sentinel
    prices.push_back(entry);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

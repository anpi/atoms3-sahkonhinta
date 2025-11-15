#ifndef TEST_DATA_HELPERS_H
#define TEST_DATA_HELPERS_H

#include "../PriceData.h"
#include <vector>

namespace TestHelpers {

// Create a price entry with specified values
PriceEntry createPriceEntry(const char* dateTime, float price, int rank = 0) {
  PriceEntry entry;
  entry.dateTime = String(dateTime);
  entry.priceWithTax = price;
  entry.rank = rank;
  return entry;
}

// Create a standard test dataset with 24 hours of 15-min prices (96 entries)
std::vector<PriceEntry> createStandardDataset() {
  std::vector<PriceEntry> prices;
  
  // Generate 96 entries for a full day (15-min intervals)
  for (int hour = 0; hour < 24; hour++) {
    for (int min = 0; min < 60; min += 15) {
      char dateTime[20];
      snprintf(dateTime, sizeof(dateTime), "2025-11-15T%02d:%02d:00", hour, min);
      
      // Price pattern: cheaper at night (01:00-06:00), expensive at peak (17:00-21:00)
      float basePrice = 0.08f; // 8 cents base
      if (hour >= 1 && hour < 6) {
        basePrice = 0.05f; // 5 cents at night
      } else if (hour >= 17 && hour < 21) {
        basePrice = 0.15f; // 15 cents at peak
      }
      
      // Add some variation
      float variation = (hour * 0.001f) + (min * 0.0001f);
      
      prices.push_back(createPriceEntry(dateTime, basePrice + variation, hour * 4 + min / 15));
    }
  }
  
  return prices;
}

// Create minimal dataset with exactly 6 entries (90 minutes)
std::vector<PriceEntry> createMinimalDataset() {
  std::vector<PriceEntry> prices;
  prices.push_back(createPriceEntry("2025-11-15T10:00:00", 0.10f, 0));
  prices.push_back(createPriceEntry("2025-11-15T10:15:00", 0.11f, 1));
  prices.push_back(createPriceEntry("2025-11-15T10:30:00", 0.12f, 2));
  prices.push_back(createPriceEntry("2025-11-15T10:45:00", 0.13f, 3));
  prices.push_back(createPriceEntry("2025-11-15T11:00:00", 0.14f, 4));
  prices.push_back(createPriceEntry("2025-11-15T11:15:00", 0.15f, 5));
  return prices;
}

// Create dataset with known cheapest period
std::vector<PriceEntry> createDatasetWithCheapestPeriod() {
  std::vector<PriceEntry> prices;
  
  // High prices at start
  for (int i = 0; i < 10; i++) {
    char dateTime[20];
    snprintf(dateTime, sizeof(dateTime), "2025-11-15T08:%02d:00", i * 15);
    prices.push_back(createPriceEntry(dateTime, 0.15f, i));
  }
  
  // Cheapest period: 10:30 - 12:00 (6 periods)
  prices.push_back(createPriceEntry("2025-11-15T10:30:00", 0.03f, 10));
  prices.push_back(createPriceEntry("2025-11-15T10:45:00", 0.04f, 11));
  prices.push_back(createPriceEntry("2025-11-15T11:00:00", 0.05f, 12));
  prices.push_back(createPriceEntry("2025-11-15T11:15:00", 0.04f, 13));
  prices.push_back(createPriceEntry("2025-11-15T11:30:00", 0.03f, 14));
  prices.push_back(createPriceEntry("2025-11-15T11:45:00", 0.05f, 15));
  
  // High prices at end
  for (int i = 0; i < 10; i++) {
    char dateTime[20];
    snprintf(dateTime, sizeof(dateTime), "2025-11-15T%02d:%02d:00", 12 + i / 4, (i % 4) * 15);
    prices.push_back(createPriceEntry(dateTime, 0.15f, 16 + i));
  }
  
  return prices;
}

} // namespace TestHelpers

#endif

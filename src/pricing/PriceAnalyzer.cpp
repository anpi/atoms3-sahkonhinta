#include "PriceAnalyzer.h"
#include <time.h>
#include <cstdio>

PriceAnalysis PriceAnalyzer::analyzePrices(const std::vector<PriceEntry>& prices) {
  PriceAnalysis result;
  
  if (prices.empty()) {
    return result;
  }
  
  int currentIdx = findCurrentPriceIndex(prices);
  if (currentIdx < 0) {
    return result;
  }
  
  result.currentPeriodStartTime = prices[currentIdx].dateTime.substring(11, 16); // Extract HH:MM
  
  // Calculate next 90 minutes average (6 periods of 15 min)
  result.next90MinAvg = calculate90MinAverage(prices, currentIdx);
  
  // Find cheapest 90 minute period
  Cheapest90Min cheapest = findCheapest90MinPeriod(prices);
  result.cheapest90MinAvg = cheapest.avg;
  if (cheapest.startIndex >= 0) {
    result.cheapest90MinTime = prices[cheapest.startIndex].dateTime.substring(11, 16);
    
    // Check if cheapest period is tomorrow
    String currentDate = prices[currentIdx].dateTime.substring(0, 10);
    String cheapestDate = prices[cheapest.startIndex].dateTime.substring(0, 10);
    result.cheapestIsTomorrow = (currentDate != cheapestDate);
  }
  
  // Only valid if we have both next 90min average and cheapest period
  result.valid = (result.next90MinAvg >= 0 && result.cheapest90MinAvg >= 0);
  return result;
}

int PriceAnalyzer::findCurrentPriceIndex(const std::vector<PriceEntry>& prices) {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  // Round down to nearest 15 minutes
  int currentMinute = (timeinfo->tm_min / 15) * 15;
  
  char currentTimeBuf[20];
  snprintf(currentTimeBuf, sizeof(currentTimeBuf), "%04d-%02d-%02dT%02d:%02d:00",
           timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
           timeinfo->tm_hour, currentMinute);
  
  String currentTime = String(currentTimeBuf);
  
  // Find matching entry
  for (size_t i = 0; i < prices.size(); i++) {
    if (prices[i].dateTime.startsWith(currentTime.substring(0, 16))) {
      return i;
    }
  }
  
  return -1;
}

float PriceAnalyzer::calculate90MinAverage(const std::vector<PriceEntry>& prices, int startIdx) {
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

Cheapest90Min PriceAnalyzer::findCheapest90MinPeriod(const std::vector<PriceEntry>& prices) {
  const int periods = 6; // 6 * 15min = 90min
  Cheapest90Min result;
  
  if (prices.size() < (size_t)periods) {
    return result; // Not enough data
  }
  
  float cheapestAvg = 999999.0f;
  int cheapestIdx = -1;
  
  // Sliding window through all available prices
  for (size_t i = 0; i <= prices.size() - periods; i++) {
    float avg = calculate90MinAverage(prices, i);
    if (avg >= 0 && avg < cheapestAvg) {
      // Check time constraints: period must start at or after 7:00 and end at or before 23:00
      // Extract hour and minute from start time (format: "YYYY-MM-DDTHH:MM:SS")
      String startTime = prices[i].dateTime.substring(11, 16); // HH:MM
      const char* startStr = startTime.c_str();
      int startHour = (startStr[0] - '0') * 10 + (startStr[1] - '0');
      
      // End time is 90 minutes (6 periods) after start
      // Last period is at index i+5, which ends 15 minutes later
      String endTime = prices[i + periods - 1].dateTime.substring(11, 16);
      const char* endStr = endTime.c_str();
      int endHour = (endStr[0] - '0') * 10 + (endStr[1] - '0');
      int endMinute = (endStr[3] - '0') * 10 + (endStr[4] - '0');
      
      // Add 15 minutes to end time to get actual end
      endMinute += 15;
      if (endMinute >= 60) {
        endMinute -= 60;
        endHour += 1;
        if (endHour >= 24) {
          endHour = 0;  // Wrap to next day
        }
      }
      
      // Check if period is within valid hours (7:00-23:00)
      // Start must be >= 7:00
      bool startValid = (startHour >= 7);
      // End must be <= 23:00 and not wrap past midnight
      bool endValid = (endHour < 23) || (endHour == 23 && endMinute == 0);
      // If end hour is less than start hour, we've crossed midnight (invalid)
      if (endHour < startHour) {
        endValid = false;
      }
      
      if (startValid && endValid) {
        cheapestAvg = avg;
        cheapestIdx = i;
      }
    }
  }
  
  if (cheapestIdx >= 0) {
    result.avg = cheapestAvg;
    result.startIndex = cheapestIdx;
  }
  
  return result;
}

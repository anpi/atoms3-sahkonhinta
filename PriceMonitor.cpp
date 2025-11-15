#include "PriceMonitor.h"
#include "PriceAnalyzer.h"
#include <ArduinoJson.h>
#include <time.h>
#include <vector>

PriceMonitor::PriceMonitor(DisplayManager* displayMgr, PriceApiClient* client) 
  : display(displayMgr), apiClient(client) {}

bool PriceMonitor::fetchAndAnalyzePrices() {
  FetchGuard guard(isFetching);
  
  if (lastAnalysis.valid) {
    display->showLoadingIndicator();
  }

  PriceApiClient::ApiResponse response = apiClient->fetchJson(API_URL);
  
  if (!response.success) {
    if (response.error == "No WiFi connection") {
      display->showText("NO WIFI");
    } else if (response.httpCode > 0) {
      display->showText("HTTP ERROR", String(response.httpCode));
    } else {
      display->showText("HTTP FAILED", response.error);
    }
    return false;
  }

  Serial.println("API Response received, parsing...");

  // Use DynamicJsonDocument for the large array
  DynamicJsonDocument doc(16384); // ~16KB for ~192 price entries
  DeserializationError error = deserializeJson(doc, response.payload);
  
  if (error) {
    display->showText("JSON ERROR", error.c_str());
    Serial.printf("JSON parse error: %s\n", error.c_str());
    return false;
  }

  if (!doc.is<JsonArray>()) {
    display->showText("JSON NOT ARRAY");
    return false;
  }

  JsonArray priceArray = doc.as<JsonArray>();
  Serial.printf("Parsed %d price entries\n", priceArray.size());
  
  // Convert to vector of PriceEntry
  std::vector<PriceEntry> prices;
  prices.reserve(priceArray.size());
  
  for (JsonObject obj : priceArray) {
    PriceEntry entry;
    entry.dateTime = obj["DateTime"].as<String>();
    entry.priceWithTax = obj["PriceWithTax"].as<float>();
    entry.rank = obj["Rank"].as<int>();
    prices.push_back(entry);
  }
  
  // Analyze prices
  lastAnalysis = PriceAnalyzer::analyzePrices(prices);
  
  if (!lastAnalysis.valid) {
    display->showText("ANALYSIS FAILED");
    return false;
  }

  Serial.printf("Current: %.2f c/kWh\n", lastAnalysis.currentPrice * 100);
  Serial.printf("Next 90min avg: %.2f c/kWh\n", lastAnalysis.next90MinAvg * 100);
  Serial.printf("Cheapest 90min: %.2f c/kWh @ %s\n", 
                lastAnalysis.cheapest90MinAvg * 100, 
                lastAnalysis.cheapest90MinTime.c_str());
  
  return true;
}

bool PriceMonitor::isScheduledUpdateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return false;
  }
  
  int currentMinute = timeinfo.tm_min;
  
  if (currentMinute == 0 || currentMinute == 15 || currentMinute == 30 || currentMinute == 45) {
    if (lastScheduledMinute != currentMinute) {
      lastScheduledMinute = currentMinute;
      Serial.printf("Scheduled update at %02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min);
      return true;
    }
  }
  
  return false;
}

const PriceAnalysis& PriceMonitor::getLastAnalysis() const {
  return lastAnalysis;
}

bool PriceMonitor::isFetchingPrice() const {
  return isFetching;
}


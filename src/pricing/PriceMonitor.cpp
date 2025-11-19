#include "PriceMonitor.h"
#include "PriceAnalyzer.h"
#include <ArduinoJson.h>
#include <time.h>
#include <vector>

PriceMonitor::PriceMonitor(IDisplay* displayMgr, IApiClient* client) 
  : display(displayMgr), apiClient(client) {}

std::vector<PriceEntry> PriceMonitor::parseJsonToEntries(const String& json) {
  std::vector<PriceEntry> prices;
  
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json.c_str());
  
  if (error) {
    Serial.printf("JSON parse error: %s\n", error.c_str());
    return prices; // empty vector indicates error
  }

  if (!doc.is<JsonArray>()) {
    Serial.println("JSON is not an array");
    return prices;
  }

  JsonArray priceArray = doc.as<JsonArray>();
  Serial.printf("Parsed %d price entries\n", priceArray.size());
  
  prices.reserve(priceArray.size());
  
  for (JsonObject obj : priceArray) {
    PriceEntry entry;
    const char* dt = obj["DateTime"];
    if (dt) entry.dateTime = String(dt);
    entry.priceWithTax = obj["PriceWithTax"].as<float>();
    prices.push_back(entry);
  }
  
  return prices;
}

void PriceMonitor::handleApiError(const IApiClient::ApiResponse& response) {
  if (response.error == "No WiFi connection") {
    display->showText("NO WIFI");
  } else if (response.httpCode > 0) {
    display->showText("HTTP ERROR", String(response.httpCode));
  } else {
    display->showText("HTTP FAILED", response.error);
  }
}

void PriceMonitor::stampAnalysisTime() {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  char timeBuf[6];
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
  lastAnalysis.lastFetchTime = String(timeBuf);
}

bool PriceMonitor::fetchAndAnalyzePrices() {
  FetchGuard guard(isFetching);
  
  if (lastAnalysis.valid) {
    display->showLoadingIndicator();
  }

  IApiClient::ApiResponse response = apiClient->fetchJson(API_URL);
  
  if (!response.success) {
    handleApiError(response);
    return false;
  }

  Serial.println("API Response received, parsing...");

  std::vector<PriceEntry> prices = this->parseJsonToEntries(response.payload);
  
  if (prices.empty()) {
    display->showText("JSON ERROR");
    return false;
  }
  
  lastAnalysis = PriceAnalyzer::analyzePrices(prices);
  
  if (!lastAnalysis.valid) {
    display->showText("ANALYSIS FAILED");
    return false;
  }
  
  stampAnalysisTime();

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


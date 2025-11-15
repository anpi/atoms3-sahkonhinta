#include "PriceMonitor.h"
#include <ArduinoJson.h>
#include <time.h>

PriceMonitor::PriceMonitor(DisplayManager* displayMgr, PriceApiClient* client) 
  : display(displayMgr), apiClient(client) {}

float PriceMonitor::fetchPrice() {
  FetchGuard guard(isFetching);
  
  if (lastPrice >= 0) {
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
    return -1;
  }

  Serial.println(response.payload);

  StaticJsonDocument<2048> doc;
  if (deserializeJson(doc, response.payload)) {
    display->showText("JSON ERROR");
    return -1;
  }

  float price = -1;
  if (doc.containsKey("PriceWithTax"))
    price = doc["PriceWithTax"].as<float>();
  else if (doc.containsKey("PriceNoTax"))
    price = doc["PriceNoTax"].as<float>();

  if (price < 0) {
    display->showText("NO PRICE FIELD");
    return -1;
  }

  if (doc.containsKey("DateTime")) {
    const char* dateTimeStr = doc["DateTime"];
    struct tm timeinfo;
    if (strptime(dateTimeStr, "%Y-%m-%dT%H:%M:%S", &timeinfo) != NULL) {
      char timeBuf[6];
      strftime(timeBuf, sizeof(timeBuf), "%H:%M", &timeinfo);
      lastUpdateTime = String(timeBuf);
    }
  }

  Serial.printf("Price: %.6f EUR/kWh\n", price);
  lastPrice = price;
  return price;
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

float PriceMonitor::getLastPrice() const {
  return lastPrice;
}

String PriceMonitor::getLastUpdateTime() const {
  return lastUpdateTime;
}

bool PriceMonitor::isFetchingPrice() const {
  return isFetching;
}

#include "WiFiManager.h"

#ifndef TESTING
#include <WiFi.h>
#include <time.h>
#endif

bool WiFiManager::connect() {
  if (wifi->getStatus() == WL_CONNECTED) {
    Serial.println("WiFi already connected: " + wifi->getLocalIP());
    return true;
  }

  wifi->setMode(WIFI_STA);
  wifi->disconnect(true);
  wifi->delayMs(500);
  
  wifi->begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Connecting to %s\n", WIFI_SSID);

  for (int i = 0; i < 40 && wifi->getStatus() != WL_CONNECTED; ++i) {
    wifi->delayMs(500);
    if (i % 5 == 0) {
      Serial.printf("WiFi status: %d\n", wifi->getStatus());
    }
  }

  if (wifi->getStatus() == WL_CONNECTED) {
    String ip = wifi->getLocalIP();
    Serial.println("WiFi OK, IP: " + ip);
    syncTime();
    return true;
  } else {
    Serial.println("WiFi FAILED, status: " + String(wifi->getStatus()));
    return false;
  }
}

void WiFiManager::disconnect() {
  if (wifi->getStatus() == WL_CONNECTED) {
    wifi->disconnect(true);
    wifi->setMode(WIFI_OFF);
    Serial.println("WiFi disconnected for power savings");
  }
}

bool WiFiManager::isConnected() {
  return wifi->getStatus() == WL_CONNECTED;
}

String WiFiManager::getIP() {
  return wifi->getLocalIP();
}

void WiFiManager::syncTime() {
  wifi->configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  Serial.println("Time sync initiated");
}

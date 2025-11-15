#include "WiFiManager.h"
#include <WiFi.h>
#include <time.h>

bool WiFiManager::connect() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi already connected: " + WiFi.localIP().toString());
    return true;
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(500);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Connecting to %s\n", WIFI_SSID);

  for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; ++i) {
    delay(500);
    if (i % 5 == 0) {
      Serial.printf("WiFi status: %d\n", WiFi.status());
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    String ip = WiFi.localIP().toString();
    Serial.println("WiFi OK, IP: " + ip);
    syncTime();
    return true;
  } else {
    Serial.println("WiFi FAILED, status: " + String(WiFi.status()));
    return false;
  }
}

void WiFiManager::disconnect() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi disconnected for power savings");
  }
}

bool WiFiManager::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getIP() {
  return WiFi.localIP().toString();
}

void WiFiManager::syncTime() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  Serial.println("Time sync initiated");
}

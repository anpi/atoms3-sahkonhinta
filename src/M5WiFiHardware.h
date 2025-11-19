#ifndef M5WIFI_HARDWARE_H
#define M5WIFI_HARDWARE_H

#include "IWiFiHardware.h"
#include <WiFi.h>
#include <time.h>

// M5AtomS3 implementation of WiFi hardware interface
class M5WiFiHardware : public IWiFiHardware {
public:
  int getStatus() override {
    return WiFi.status();
  }
  
  void setMode(int mode) override {
    WiFi.mode((wifi_mode_t)mode);
  }
  
  void disconnect(bool wifiOff) override {
    WiFi.disconnect(wifiOff);
  }
  
  void begin(const char* ssid, const char* pass) override {
    WiFi.begin(ssid, pass);
  }
  
  String getLocalIP() override {
    return WiFi.localIP().toString();
  }
  
  void delayMs(unsigned long ms) override {
    delay(ms);
  }
  
  void configTime(long gmtOffset, int daylightOffset, const char* server) override {
    ::configTime(gmtOffset, daylightOffset, server);
  }
};

#endif // M5WIFI_HARDWARE_H

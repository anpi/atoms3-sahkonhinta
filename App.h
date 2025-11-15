#ifndef APP_H
#define APP_H

#include "DisplayManager.h"
#include "WiFiManager.h"
#include "PriceApiClient.h"
#include "PriceMonitor.h"
#include "TimerManager.h"
#include "IdleManager.h"

extern const char* WIFI_SSID;

class App {
private:
  DisplayManager displayManager;
  WiFiManager wifiManager;
  PriceApiClient apiClient;
  PriceMonitor priceMonitor;
  TimerManager timerManager;
  IdleManager idleManager;

  float fetchPriceWithWifi();
  void handleButtonPress();
  void handleScheduledUpdate();

public:
  App();
  void setup();
  void loop();
};

#endif

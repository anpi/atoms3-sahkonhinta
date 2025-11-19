#ifndef APP_H
#define APP_H

#include "M5DisplayHardware.h"
#include "DisplayManager.h"
#include "WiFiManager.h"
#include "PriceApiClient.h"
#include "PriceMonitor.h"
#include "TimerManager.h"
#include "IdleManager.h"

extern const char* WIFI_SSID;

class App {
private:
  M5DisplayHardware displayHardware;
  DisplayManager displayManager;
  WiFiManager wifiManager;
  PriceApiClient apiClient;
  PriceMonitor priceMonitor;
  TimerManager timerManager;
  IdleManager idleManager;

  bool fetchPriceWithWifi();
  void handleButtonPress();
  void handleScheduledUpdate();

public:
  App();
  void setup();
  void loop();
};

#endif

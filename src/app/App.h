#ifndef APP_H
#define APP_H

#include "../display/M5DisplayHardware.h"
#include "../display/DisplayManager.h"
#include "../timing/M5TimerHardware.h"
#include "../network/M5WiFiHardware.h"
#include "../network/WiFiManager.h"
#include "../pricing/PriceApiClient.h"
#include "../pricing/PriceMonitor.h"
#include "../timing/TimerManager.h"
#include "IdleManager.h"

extern const char* WIFI_SSID;

class App {
private:
  M5DisplayHardware displayHardware;
  DisplayManager displayManager;
  M5TimerHardware timerHardware;
  M5WiFiHardware wifiHardware;
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

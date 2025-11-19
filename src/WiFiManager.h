#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#ifndef WString_h
#include <WString.h>
#endif
#include "IWiFiHardware.h"

extern const char* WIFI_SSID;
extern const char* WIFI_PASS;
extern const char* NTP_SERVER;
extern const long GMT_OFFSET_SEC;
extern const int DAYLIGHT_OFFSET_SEC;

class WiFiManager {
public:
  WiFiManager(IWiFiHardware* wifi) : wifi(wifi) {}
  
  bool connect();
  void disconnect();
  bool isConnected();
  String getIP();

private:
  void syncTime();
  IWiFiHardware* wifi;
};

#endif

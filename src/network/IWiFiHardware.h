#ifndef IWIFI_HARDWARE_H
#define IWIFI_HARDWARE_H

#ifndef WString_h
#include <WString.h>
#endif

// Hardware abstraction layer for WiFi operations
class IWiFiHardware {
public:
  virtual ~IWiFiHardware() = default;
  
  // WiFi operations
  virtual int getStatus() = 0;
  virtual void setMode(int mode) = 0;
  virtual void disconnect(bool wifiOff) = 0;
  virtual void begin(const char* ssid, const char* pass) = 0;
  virtual String getLocalIP() = 0;
  virtual void delayMs(unsigned long ms) = 0;
  
  // Time synchronization
  virtual void configTime(long gmtOffset, int daylightOffset, const char* server) = 0;
};

#endif // IWIFI_HARDWARE_H

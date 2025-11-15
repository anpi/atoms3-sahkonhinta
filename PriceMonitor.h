#ifndef PRICE_MONITOR_H
#define PRICE_MONITOR_H

#include <WString.h>
#include "DisplayManager.h"
#include "PriceApiClient.h"
#include "FetchGuard.h"

extern const char* API_URL;

class PriceMonitor {
private:
  float lastPrice = -1;
  String lastUpdateTime = "--:--";
  int lastScheduledMinute = -1;
  bool isFetching = false;
  DisplayManager* display;
  PriceApiClient* apiClient;

public:
  PriceMonitor(DisplayManager* displayMgr, PriceApiClient* client);
  float fetchPrice();
  bool isScheduledUpdateTime();
  float getLastPrice() const;
  String getLastUpdateTime() const;
  bool isFetchingPrice() const;
};

#endif

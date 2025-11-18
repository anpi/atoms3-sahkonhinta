#ifndef PRICE_MONITOR_H
#define PRICE_MONITOR_H

#include <WString.h>
#include "DisplayManager.h"
#include "PriceApiClient.h"
#include "PriceData.h"
#include "FetchGuard.h"

extern const char* API_URL;

class PriceMonitor {
private:
  PriceAnalysis lastAnalysis;
  int lastScheduledMinute = -1;
  bool isFetching = false;
  DisplayManager* display;
  PriceApiClient* apiClient;

  // Helper methods for testability
  std::vector<PriceEntry> parseJsonToEntries(const String& json);
  void handleApiError(const PriceApiClient::ApiResponse& response);
  void stampAnalysisTime();

public:
  PriceMonitor(DisplayManager* displayMgr, PriceApiClient* client);
  bool fetchAndAnalyzePrices();
  bool isScheduledUpdateTime();
  const PriceAnalysis& getLastAnalysis() const;
  bool isFetchingPrice() const;
};

#endif

#ifndef PRICE_MONITOR_H
#define PRICE_MONITOR_H

#include <vector>
#ifndef WString_h
#include <WString.h>
#endif
#include "../display/IDisplay.h"
#include "IApiClient.h"
#include "PriceData.h"
#include "FetchGuard.h"

extern const char* API_URL;

class PriceMonitor {
private:
  PriceAnalysis lastAnalysis;
  int lastScheduledMinute = -1;
  bool isFetching = false;
  IDisplay* display;
  IApiClient* apiClient;

protected:
  // Helper methods for testability
  std::vector<PriceEntry> parseJsonToEntries(const String& json);
  void handleApiError(const IApiClient::ApiResponse& response);
  void stampAnalysisTime();

public:
  PriceMonitor(IDisplay* displayMgr, IApiClient* client);
  bool fetchAndAnalyzePrices();
  bool isScheduledUpdateTime();
  const PriceAnalysis& getLastAnalysis() const;
  bool isFetchingPrice() const;
};

#endif

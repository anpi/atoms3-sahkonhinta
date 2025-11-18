#ifndef MOCK_PRICE_API_CLIENT_H
#define MOCK_PRICE_API_CLIENT_H

#include "../TestStringAdapter.h"

class PriceApiClient {
public:
  struct ApiResponse {
    bool success;
    String payload;
    int httpCode;
    String error;
  };

  ApiResponse fetchJson(const char*) {
    return {false, "", 0, "mock"};
  }
};

#endif

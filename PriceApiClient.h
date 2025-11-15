#ifndef PRICE_API_CLIENT_H
#define PRICE_API_CLIENT_H

#include <WString.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

class PriceApiClient {
public:
  struct ApiResponse {
    bool success;
    String payload;
    int httpCode;
    String error;
  };

  ApiResponse fetchJson(const char* url);
};

#endif

#ifndef PRICE_API_CLIENT_H
#define PRICE_API_CLIENT_H

#include <WString.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "IApiClient.h"

class PriceApiClient : public IApiClient {
public:
  ApiResponse fetchJson(const char* url) override;
};

#endif

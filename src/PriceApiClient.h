#ifndef PRICE_API_CLIENT_H
#define PRICE_API_CLIENT_H

#ifndef WString_h
#include <WString.h>
#endif
#ifndef WIFI_CLIENT_SECURE_H
#include <WiFiClientSecure.h>
#endif
#ifndef HTTP_CLIENT_H
#include <HTTPClient.h>
#endif
#include "IApiClient.h"

class PriceApiClient : public IApiClient {
public:
  ApiResponse fetchJson(const char* url) override;
};

#endif

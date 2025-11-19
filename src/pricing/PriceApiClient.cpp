#include "PriceApiClient.h"
#ifndef WiFi_h
#include <WiFi.h>
#endif

PriceApiClient::ApiResponse PriceApiClient::fetchJson(const char* url) {
  ApiResponse response;
  response.success = false;
  response.httpCode = 0;

  if (WiFi.status() != WL_CONNECTED) {
    response.error = "No WiFi connection";
    return response;
  }

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  if (!http.begin(client, url)) {
    response.error = "HTTP begin failed";
    return response;
  }

  response.httpCode = http.GET();
  if (response.httpCode != 200) {
    response.error = String("HTTP error ") + String(response.httpCode);
    http.end();
    return response;
  }

  response.payload = http.getString();
  http.end();
  response.success = true;
  return response;
}

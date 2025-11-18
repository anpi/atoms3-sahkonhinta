#ifndef MOCK_API_CLIENT_H
#define MOCK_API_CLIENT_H

#include "../../src/IApiClient.h"
#include "../TestStringAdapter.h"

/**
 * Mock implementation of IApiClient for testing.
 * Returns predefined responses without network calls.
 */
class MockApiClient : public IApiClient {
public:
  ApiResponse nextResponse;
  String lastUrl;
  int fetchCallCount = 0;

  MockApiClient() {
    // Default to success with empty payload
    nextResponse.success = true;
    nextResponse.payload = "";
    nextResponse.httpCode = 200;
    nextResponse.error = "";
  }

  ApiResponse fetchJson(const char* url) override {
    lastUrl = String(url);
    fetchCallCount++;
    return nextResponse;
  }

  void setSuccessResponse(const String& json) {
    nextResponse.success = true;
    nextResponse.payload = json;
    nextResponse.httpCode = 200;
    nextResponse.error = "";
  }

  void setErrorResponse(int httpCode, const String& error) {
    nextResponse.success = false;
    nextResponse.payload = "";
    nextResponse.httpCode = httpCode;
    nextResponse.error = error;
  }
};

#endif

#ifndef IAPI_CLIENT_H
#define IAPI_CLIENT_H

#ifndef WString_h
#include <WString.h>
#endif

/**
 * Interface for API client operations.
 * Allows mocking in tests without network dependencies.
 */
class IApiClient {
public:
  struct ApiResponse {
    bool success;
    String payload;
    int httpCode;
    String error;
  };

  virtual ~IApiClient() = default;
  virtual ApiResponse fetchJson(const char* url) = 0;
};

#endif

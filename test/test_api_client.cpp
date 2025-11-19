#include <gtest/gtest.h>

// Use test String adapter
#include "TestStringAdapter.h"
#define WString_h
#define WIFI_CLIENT_SECURE_H
#define HTTP_CLIENT_H

// Mock WiFi status
#define WL_CONNECTED 3
namespace {
  struct MockWiFiClass {
    int status() { return mockStatus; }
    static int mockStatus;
  } WiFi;
  int MockWiFiClass::mockStatus = WL_CONNECTED;
}

// Mock WiFiClientSecure
class WiFiClientSecure {
public:
  void setInsecure() {}
};

// Mock HTTPClient
class HTTPClient {
public:
  bool begin(WiFiClientSecure&, const char*) { return mockBeginSuccess; }
  int GET() { return mockHttpCode; }
  String getString() { return mockPayload; }
  void end() {}
  
  static bool mockBeginSuccess;
  static int mockHttpCode;
  static String mockPayload;
};

bool HTTPClient::mockBeginSuccess = true;
int HTTPClient::mockHttpCode = 200;
String HTTPClient::mockPayload = "{}";

// Prevent including real WiFi.h
#define WiFi_h

// Include the actual implementation
#include "../src/IApiClient.h"
#include "../src/PriceApiClient.cpp"

// Test Suite: WiFi Status Checking
TEST(PriceApiClient, ReturnsErrorWhenWiFiNotConnected) {
  PriceApiClient client;
  MockWiFiClass::mockStatus = 0; // Not connected
  
  auto response = client.fetchJson("http://example.com");
  
  EXPECT_FALSE(response.success);
  EXPECT_EQ(response.error, String("No WiFi connection"));
  EXPECT_EQ(response.httpCode, 0);
}

TEST(PriceApiClient, ProceedsWhenWiFiConnected) {
  PriceApiClient client;
  MockWiFiClass::mockStatus = WL_CONNECTED;
  HTTPClient::mockBeginSuccess = true;
  HTTPClient::mockHttpCode = 200;
  HTTPClient::mockPayload = "{\"test\":\"data\"}";
  
  auto response = client.fetchJson("http://example.com");
  
  EXPECT_TRUE(response.success);
  EXPECT_EQ(response.httpCode, 200);
  EXPECT_EQ(response.payload, String("{\"test\":\"data\"}"));
}

// Test Suite: HTTP Begin Failures
TEST(PriceApiClient, HandlesHttpBeginFailure) {
  PriceApiClient client;
  MockWiFiClass::mockStatus = WL_CONNECTED;
  HTTPClient::mockBeginSuccess = false;
  
  auto response = client.fetchJson("http://example.com");
  
  EXPECT_FALSE(response.success);
  EXPECT_EQ(response.error, String("HTTP begin failed"));
  EXPECT_EQ(response.httpCode, 0);
}

// Test Suite: HTTP Status Code Handling
TEST(PriceApiClient, Returns200SuccessResponse) {
  PriceApiClient client;
  MockWiFiClass::mockStatus = WL_CONNECTED;
  HTTPClient::mockBeginSuccess = true;
  HTTPClient::mockHttpCode = 200;
  HTTPClient::mockPayload = "success_payload";
  
  auto response = client.fetchJson("http://example.com");
  
  EXPECT_TRUE(response.success);
  EXPECT_EQ(response.httpCode, 200);
  EXPECT_EQ(response.payload, String("success_payload"));
  EXPECT_TRUE(response.error.length() == 0);
}

TEST(PriceApiClient, Handles404NotFound) {
  PriceApiClient client;
  MockWiFiClass::mockStatus = WL_CONNECTED;
  HTTPClient::mockBeginSuccess = true;
  HTTPClient::mockHttpCode = 404;
  
  auto response = client.fetchJson("http://example.com");
  
  EXPECT_FALSE(response.success);
  EXPECT_EQ(response.httpCode, 404);
  EXPECT_EQ(response.error, String("HTTP error 404"));
}

TEST(PriceApiClient, Handles500ServerError) {
  PriceApiClient client;
  MockWiFiClass::mockStatus = WL_CONNECTED;
  HTTPClient::mockBeginSuccess = true;
  HTTPClient::mockHttpCode = 500;
  
  auto response = client.fetchJson("http://example.com");
  
  EXPECT_FALSE(response.success);
  EXPECT_EQ(response.httpCode, 500);
  EXPECT_EQ(response.error, String("HTTP error 500"));
}

TEST(PriceApiClient, Handles401Unauthorized) {
  PriceApiClient client;
  MockWiFiClass::mockStatus = WL_CONNECTED;
  HTTPClient::mockBeginSuccess = true;
  HTTPClient::mockHttpCode = 401;
  
  auto response = client.fetchJson("http://example.com");
  
  EXPECT_FALSE(response.success);
  EXPECT_EQ(response.httpCode, 401);
  EXPECT_EQ(response.error, String("HTTP error 401"));
}

TEST(PriceApiClient, Handles503ServiceUnavailable) {
  PriceApiClient client;
  MockWiFiClass::mockStatus = WL_CONNECTED;
  HTTPClient::mockBeginSuccess = true;
  HTTPClient::mockHttpCode = 503;
  
  auto response = client.fetchJson("http://example.com");
  
  EXPECT_FALSE(response.success);
  EXPECT_EQ(response.httpCode, 503);
  EXPECT_EQ(response.error, String("HTTP error 503"));
}

// Test Suite: Edge Cases
TEST(PriceApiClient, HandlesEmptyPayload) {
  PriceApiClient client;
  MockWiFiClass::mockStatus = WL_CONNECTED;
  HTTPClient::mockBeginSuccess = true;
  HTTPClient::mockHttpCode = 200;
  HTTPClient::mockPayload = "";
  
  auto response = client.fetchJson("http://example.com");
  
  EXPECT_TRUE(response.success);
  EXPECT_EQ(response.httpCode, 200);
  EXPECT_EQ(response.payload, String(""));
}

TEST(PriceApiClient, HandlesLargePayload) {
  PriceApiClient client;
  MockWiFiClass::mockStatus = WL_CONNECTED;
  HTTPClient::mockBeginSuccess = true;
  HTTPClient::mockHttpCode = 200;
  
  std::string large(5000, 'x');
  HTTPClient::mockPayload = String(large.c_str());
  
  auto response = client.fetchJson("http://example.com");
  
  EXPECT_TRUE(response.success);
  EXPECT_EQ(response.httpCode, 200);
  EXPECT_EQ(response.payload.length(), 5000u);
}

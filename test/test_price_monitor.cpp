#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>

using ::testing::_;
using ::testing::Return;
using ::testing::Eq;
using ::testing::InSequence;

// Test String adapter
#include "TestStringAdapter.h"
#define WString_h

// Mock time functions
static int mock_hour = 12;
static int mock_minute = 30;

extern "C" {
  time_t time(time_t* t) {
    time_t now = 1700000000;
    if (t) *t = now;
    return now;
  }
  
  struct tm* localtime(const time_t*) {
    static struct tm timeinfo;
    timeinfo.tm_year = 2025 - 1900;  // tm_year is years since 1900
    timeinfo.tm_mon = 11 - 1;         // tm_mon is 0-11
    timeinfo.tm_mday = 18;
    timeinfo.tm_hour = mock_hour;
    timeinfo.tm_min = mock_minute;
    timeinfo.tm_sec = 0;
    return &timeinfo;
  }
  
  bool getLocalTime(struct tm* info) {
    info->tm_year = 2025 - 1900;
    info->tm_mon = 11 - 1;
    info->tm_mday = 18;
    info->tm_hour = mock_hour;
    info->tm_min = mock_minute;
    info->tm_sec = 0;
    return true;
  }
}

// Mock Serial
namespace {
  struct MockSerial {
    void printf(const char*, ...) {}
    void println(const char*) {}
  } Serial;
}

#include <ArduinoJson.h>
#include "../src/PriceData.h"
#include "../src/FetchGuard.h"
#include "../src/IDisplay.h"
#include "../src/IApiClient.h"

// gMock display
class MockDisplay : public IDisplay {
public:
  MOCK_METHOD(void, initialize, (), (override));
  MOCK_METHOD(void, showText, (const String& l1, const String& l2), (override));
  MOCK_METHOD(void, showLoadingIndicator, (), (override));
  MOCK_METHOD(void, showWifiIndicator, (), (override));
  MOCK_METHOD(void, showAnalysis, (const PriceAnalysis& analysis), (override));
  MOCK_METHOD(void, setBrightness, (bool shouldBeBright), (override));
  MOCK_METHOD(void, setBrightUntil, (unsigned long until), (override));
  MOCK_METHOD(void, updateBrightness, (bool isFetching), (override));
  MOCK_METHOD(bool, isBright, (), (const, override));
};

// gMock API client
class MockApiClient : public IApiClient {
public:
  MOCK_METHOD(ApiResponse, fetchJson, (const char* url), (override));
};

extern const char* API_URL;
const char* API_URL = "mock://api";

#include "../src/PriceAnalyzer.cpp"
#include "../src/PriceMonitor.cpp"

// Helper to generate valid test JSON with 15-minute intervals
String generateValidPriceJson() {
  return R"([
    {"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.16},
    {"DateTime":"2025-11-18T10:15:00","PriceWithTax":0.16},
    {"DateTime":"2025-11-18T10:30:00","PriceWithTax":0.16},
    {"DateTime":"2025-11-18T10:45:00","PriceWithTax":0.17},
    {"DateTime":"2025-11-18T11:00:00","PriceWithTax":0.17},
    {"DateTime":"2025-11-18T11:15:00","PriceWithTax":0.17},
    {"DateTime":"2025-11-18T11:30:00","PriceWithTax":0.17},
    {"DateTime":"2025-11-18T11:45:00","PriceWithTax":0.18},
    {"DateTime":"2025-11-18T12:00:00","PriceWithTax":0.18},
    {"DateTime":"2025-11-18T12:15:00","PriceWithTax":0.18},
    {"DateTime":"2025-11-18T12:30:00","PriceWithTax":0.18},
    {"DateTime":"2025-11-18T12:45:00","PriceWithTax":0.19},
    {"DateTime":"2025-11-18T13:00:00","PriceWithTax":0.20},
    {"DateTime":"2025-11-18T13:15:00","PriceWithTax":0.20},
    {"DateTime":"2025-11-18T13:30:00","PriceWithTax":0.21},
    {"DateTime":"2025-11-18T13:45:00","PriceWithTax":0.10},
    {"DateTime":"2025-11-18T14:00:00","PriceWithTax":0.10},
    {"DateTime":"2025-11-18T14:15:00","PriceWithTax":0.10},
    {"DateTime":"2025-11-18T14:30:00","PriceWithTax":0.10},
    {"DateTime":"2025-11-18T14:45:00","PriceWithTax":0.11},
    {"DateTime":"2025-11-18T15:00:00","PriceWithTax":0.21},
    {"DateTime":"2025-11-18T15:15:00","PriceWithTax":0.21},
    {"DateTime":"2025-11-18T15:30:00","PriceWithTax":0.20},
    {"DateTime":"2025-11-18T15:45:00","PriceWithTax":0.20},
    {"DateTime":"2025-11-18T16:00:00","PriceWithTax":0.19}
  ])";
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST(PriceMonitor, FetchAndAnalyze_NoWiFi_ShowsError) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  IApiClient::ApiResponse response;
  response.success = false;
  response.error = "No WiFi connection";
  response.httpCode = 0;
  
  EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  EXPECT_CALL(mockDisplay, showText(Eq("NO WIFI"), _)).Times(1);
  
  bool result = monitor.fetchAndAnalyzePrices();
  
  EXPECT_FALSE(result);
}

TEST(PriceMonitor, FetchAndAnalyze_HttpError_ShowsStatusCode) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  IApiClient::ApiResponse response;
  response.success = false;
  response.error = "Not Found";
  response.httpCode = 404;
  
  EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  EXPECT_CALL(mockDisplay, showText(Eq("HTTP ERROR"), Eq("404"))).Times(1);
  
  bool result = monitor.fetchAndAnalyzePrices();
  
  EXPECT_FALSE(result);
}

TEST(PriceMonitor, FetchAndAnalyze_HttpFailure_ShowsError) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  IApiClient::ApiResponse response;
  response.success = false;
  response.error = "Connection timeout";
  response.httpCode = 0;
  
  EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  EXPECT_CALL(mockDisplay, showText(Eq("HTTP FAILED"), Eq("Connection timeout"))).Times(1);
  
  bool result = monitor.fetchAndAnalyzePrices();
  
  EXPECT_FALSE(result);
}

TEST(PriceMonitor, FetchAndAnalyze_InvalidJson_ShowsError) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  IApiClient::ApiResponse response;
  response.success = true;
  response.payload = "{ not valid json }";
  response.httpCode = 200;
  
  EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  EXPECT_CALL(mockDisplay, showText(Eq("JSON ERROR"), _)).Times(1);
  
  bool result = monitor.fetchAndAnalyzePrices();
  
  EXPECT_FALSE(result);
}

TEST(PriceMonitor, FetchAndAnalyze_EmptyArray_ShowsError) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  IApiClient::ApiResponse response;
  response.success = true;
  response.payload = "[]";
  response.httpCode = 200;
  
  EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  EXPECT_CALL(mockDisplay, showText(Eq("JSON ERROR"), _)).Times(1);
  
  bool result = monitor.fetchAndAnalyzePrices();
  
  EXPECT_FALSE(result);
}

TEST(PriceMonitor, FetchAndAnalyze_InsufficientData_ShowsAnalysisFailed) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  // Only 2 entries - not enough for analysis
  String minimalJson = R"([
    {"DateTime":"2025-11-18T12:00:00","PriceWithTax":0.10},
    {"DateTime":"2025-11-18T12:15:00","PriceWithTax":0.11}
  ])";
  
  IApiClient::ApiResponse response;
  response.success = true;
  response.payload = minimalJson;
  response.httpCode = 200;
  
  EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  EXPECT_CALL(mockDisplay, showText(Eq("ANALYSIS FAILED"), _)).Times(1);
  
  bool result = monitor.fetchAndAnalyzePrices();
  
  EXPECT_FALSE(result);
}

// ============================================================================
// Success Path Tests
// ============================================================================

TEST(PriceMonitor, FetchAndAnalyze_Success_ReturnsTrue) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  IApiClient::ApiResponse response;
  response.success = true;
  response.payload = generateValidPriceJson();
  response.httpCode = 200;
  
  EXPECT_CALL(mockApiClient, fetchJson("mock://api")).WillOnce(Return(response));
  
  bool result = monitor.fetchAndAnalyzePrices();
  
  EXPECT_TRUE(result);
  EXPECT_TRUE(monitor.getLastAnalysis().valid);
}

TEST(PriceMonitor, FetchAndAnalyze_Success_StampsTime) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  mock_hour = 14;
  mock_minute = 25;
  
  IApiClient::ApiResponse response;
  response.success = true;
  response.payload = generateValidPriceJson();
  response.httpCode = 200;
  
  EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  
  monitor.fetchAndAnalyzePrices();
  
  EXPECT_EQ(monitor.getLastAnalysis().lastFetchTime, String("14:25"));
}

TEST(PriceMonitor, FetchAndAnalyze_SecondFetch_ShowsLoadingIndicator) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  IApiClient::ApiResponse response;
  response.success = true;
  response.payload = generateValidPriceJson();
  response.httpCode = 200;
  
  // First fetch - no loading indicator
  EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  monitor.fetchAndAnalyzePrices();
  
  // Second fetch - should show loading indicator
  EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  EXPECT_CALL(mockDisplay, showLoadingIndicator()).Times(1);
  
  monitor.fetchAndAnalyzePrices();
}

// ============================================================================
// Scheduling Tests
// ============================================================================

TEST(PriceMonitor, IsScheduledUpdateTime_AtQuarterHour_ReturnsTrue) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  mock_hour = 14;
  mock_minute = 15;
  
  EXPECT_TRUE(monitor.isScheduledUpdateTime());
}

TEST(PriceMonitor, IsScheduledUpdateTime_AtHour_ReturnsTrue) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  mock_hour = 14;
  mock_minute = 0;
  
  EXPECT_TRUE(monitor.isScheduledUpdateTime());
}

TEST(PriceMonitor, IsScheduledUpdateTime_BetweenQuarters_ReturnsFalse) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  mock_hour = 14;
  mock_minute = 17;
  
  EXPECT_FALSE(monitor.isScheduledUpdateTime());
}

TEST(PriceMonitor, IsScheduledUpdateTime_SameMinuteTwice_ReturnsFalseSecondTime) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  mock_hour = 14;
  mock_minute = 30;
  
  EXPECT_TRUE(monitor.isScheduledUpdateTime());
  EXPECT_FALSE(monitor.isScheduledUpdateTime());  // Same minute
}

TEST(PriceMonitor, IsScheduledUpdateTime_DifferentQuarters_BothReturnTrue) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  mock_hour = 14;
  mock_minute = 30;
  
  EXPECT_TRUE(monitor.isScheduledUpdateTime());
  
  mock_minute = 45;
  EXPECT_TRUE(monitor.isScheduledUpdateTime());
}

// ============================================================================
// State Tests
// ============================================================================

TEST(PriceMonitor, IsFetchingPrice_InitiallyFalse) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  EXPECT_FALSE(monitor.isFetchingPrice());
}

TEST(PriceMonitor, GetLastAnalysis_InitiallyInvalid) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  EXPECT_FALSE(monitor.getLastAnalysis().valid);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST(PriceMonitor, FetchAndAnalyze_CallsApiWithCorrectUrl) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  IApiClient::ApiResponse response;
  response.success = true;
  response.payload = generateValidPriceJson();
  response.httpCode = 200;
  
  EXPECT_CALL(mockApiClient, fetchJson(Eq("mock://api"))).WillOnce(Return(response));
  
  monitor.fetchAndAnalyzePrices();
}

TEST(PriceMonitor, FetchAndAnalyze_LoadingIndicatorBeforeApi) {
  MockDisplay mockDisplay;
  MockApiClient mockApiClient;
  PriceMonitor monitor(&mockDisplay, &mockApiClient);
  
  IApiClient::ApiResponse response;
  response.success = true;
  response.payload = generateValidPriceJson();
  response.httpCode = 200;
  
  // Prime with first fetch
  EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  monitor.fetchAndAnalyzePrices();
  
  // Second fetch should show loading before calling API
  {
    InSequence seq;
    EXPECT_CALL(mockDisplay, showLoadingIndicator());
    EXPECT_CALL(mockApiClient, fetchJson(_)).WillOnce(Return(response));
  }
  
  monitor.fetchAndAnalyzePrices();
}

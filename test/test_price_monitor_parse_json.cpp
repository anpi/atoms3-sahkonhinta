#include <gtest/gtest.h>
#include <vector>

// Use test String adapter before including production headers
#include "TestStringAdapter.h"
#define WString_h  // Prevent Arduino WString.h inclusion

// Mock Serial for testing
namespace {
  struct MockSerial {
    void printf(const char*, ...) {}
    void println(const char*) {}
  } Serial;
}

// Mock time functions for PriceMonitor
extern "C" {
  time_t time(time_t* t) {
    time_t now = 1700000000; // Fixed timestamp
    if (t) *t = now;
    return now;
  }
  
  struct tm* localtime(const time_t* /* timer */) {
    static struct tm timeinfo;
    timeinfo.tm_hour = 12;
    timeinfo.tm_min = 30;
    return &timeinfo;
  }
  
  bool getLocalTime(struct tm* info) {
    info->tm_hour = 12;
    info->tm_min = 30;
    return true;
  }
}

// Include ArduinoJson
#include <ArduinoJson.h>

#include "../src/PriceData.h"
#include "../src/FetchGuard.h"

// Now include mocks and real implementations
#include "mocks/MockDisplay.h"
#include "mocks/MockApiClient.h"

extern const char* API_URL;
const char* API_URL = "mock";

// Include actual PriceAnalyzer and PriceMonitor implementations
#include "../src/PriceAnalyzer.cpp"
#include "../src/PriceMonitor.cpp"

// Test wrapper to access private methods
class PriceMonitorTestWrapper : public PriceMonitor {
public:
  PriceMonitorTestWrapper() : PriceMonitor(nullptr, nullptr) {}
  
  std::vector<PriceEntry> testParseJsonToEntries(const String& json) {
    return parseJsonToEntries(json);
  }
};

// Test Suite: Core Functionality
TEST(ParseJsonToEntries, ValidJsonArray_BasicEntries) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([
    {"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.10},
    {"DateTime":"2025-11-18T10:15:00","PriceWithTax":0.11},
    {"DateTime":"2025-11-18T10:30:00","PriceWithTax":0.12}
  ])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(3));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00"));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.10f);
  EXPECT_EQ(result[1].dateTime, String("2025-11-18T10:15:00"));
  EXPECT_FLOAT_EQ(result[1].priceWithTax, 0.11f);
  EXPECT_EQ(result[2].dateTime, String("2025-11-18T10:30:00"));
  EXPECT_FLOAT_EQ(result[2].priceWithTax, 0.12f);
}

TEST(ParseJsonToEntries, ValidJsonArray_SingleEntry) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.15}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00"));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.15f);
}

TEST(ParseJsonToEntries, ValidJsonArray_EmptyArray) {
  PriceMonitorTestWrapper harness;
  
  String json = "[]";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

TEST(ParseJsonToEntries, ValidJsonArray_LargeArray) {
  PriceMonitorTestWrapper harness;
  
  // Build JSON with 50 entries
  std::string json = "[";
  for (int i = 0; i < 50; i++) {
    if (i > 0) json += ",";
    json += R"({"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.10})";
  }
  json += "]";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(String(json.c_str()));
  
  EXPECT_EQ(result.size(), static_cast<size_t>(50));
}

// Test Suite: Error Handling
TEST(ParseJsonToEntries, InvalidJson_MalformedSyntax) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.10,}])"; // trailing comma
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0)); // empty = error
}

TEST(ParseJsonToEntries, InvalidJson_NotArray_Object) {
  PriceMonitorTestWrapper harness;
  
  String json = R"({"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.10})";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

TEST(ParseJsonToEntries, InvalidJson_NotArray_String) {
  PriceMonitorTestWrapper harness;
  
  String json = R"("not an array")";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

TEST(ParseJsonToEntries, InvalidJson_CompletelyBroken) {
  PriceMonitorTestWrapper harness;
  
  String json = "this is not json at all!@#$%";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

TEST(ParseJsonToEntries, InvalidJson_EmptyString) {
  PriceMonitorTestWrapper harness;
  
  String json = "";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

TEST(ParseJsonToEntries, InvalidJson_UnclosedBracket) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.10})";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

// Test Suite: Field Validation
TEST(ParseJsonToEntries, MissingField_DateTime) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"PriceWithTax":0.10}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("")); // Empty string for missing field
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.10f);
}

TEST(ParseJsonToEntries, MissingField_PriceWithTax) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00"}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00"));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.0f); // Default value
}

TEST(ParseJsonToEntries, MissingFields_Both) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{}])"; // Empty object
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String(""));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.0f);
}

TEST(ParseJsonToEntries, ExtraFields_ShouldIgnore) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{
    "DateTime":"2025-11-18T10:00:00",
    "PriceWithTax":0.10,
    "ExtraField":"ignored",
    "AnotherField":123
  }])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00"));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.10f);
}

// Test Suite: Data Types
TEST(ParseJsonToEntries, WrongType_PriceAsString) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":"0.10"}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  // ArduinoJson will attempt conversion
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.10f);
}

TEST(ParseJsonToEntries, NegativePrice) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":-0.05}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, -0.05f);
}

TEST(ParseJsonToEntries, VeryLargePrice) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":999999.99}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_NEAR(result[0].priceWithTax, 999999.99f, 0.01f);
}

TEST(ParseJsonToEntries, ZeroPrice) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.0}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.0f);
}

// Test Suite: DateTime Formats
TEST(ParseJsonToEntries, DateTimeFormat_WithMilliseconds) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00.000","PriceWithTax":0.10}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00.000"));
}

TEST(ParseJsonToEntries, DateTimeFormat_WithTimezone) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00+02:00","PriceWithTax":0.10}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00+02:00"));
}

TEST(ParseJsonToEntries, DateTimeFormat_Unusual) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":"18-11-2025 10:00","PriceWithTax":0.10}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("18-11-2025 10:00"));
}

// Test Suite: Edge Cases
TEST(ParseJsonToEntries, WhitespaceVariations) {
  PriceMonitorTestWrapper harness;
  
  String json = R"(
    [
      {
        "DateTime"  :  "2025-11-18T10:00:00"  ,
        "PriceWithTax"  :  0.10
      }
    ]
  )";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00"));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.10f);
}

TEST(ParseJsonToEntries, MultipleEntries_MixedValues) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([
    {"DateTime":"2025-11-18T00:00:00","PriceWithTax":0.05},
    {"DateTime":"2025-11-18T06:00:00","PriceWithTax":0.12},
    {"DateTime":"2025-11-18T12:00:00","PriceWithTax":0.18},
    {"DateTime":"2025-11-18T18:00:00","PriceWithTax":0.15},
    {"DateTime":"2025-11-18T23:45:00","PriceWithTax":0.08}
  ])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(5));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.05f);
  EXPECT_FLOAT_EQ(result[2].priceWithTax, 0.18f);
  EXPECT_FLOAT_EQ(result[4].priceWithTax, 0.08f);
}

TEST(ParseJsonToEntries, RealWorldExample_TypicalApiResponse) {
  PriceMonitorTestWrapper harness;
  
  // Simulate typical API response with realistic data
  String json = R"([
    {"DateTime":"2025-11-18T00:00:00+02:00","PriceWithTax":0.0543},
    {"DateTime":"2025-11-18T00:15:00+02:00","PriceWithTax":0.0521},
    {"DateTime":"2025-11-18T00:30:00+02:00","PriceWithTax":0.0498},
    {"DateTime":"2025-11-18T00:45:00+02:00","PriceWithTax":0.0476}
  ])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(4));
  EXPECT_TRUE(result[0].dateTime.startsWith(String("2025-11-18T00:00")));
  EXPECT_NEAR(result[0].priceWithTax, 0.0543f, 0.0001f);
  EXPECT_NEAR(result[3].priceWithTax, 0.0476f, 0.0001f);
}

TEST(ParseJsonToEntries, NullValues_InObject) {
  PriceMonitorTestWrapper harness;
  
  String json = R"([{"DateTime":null,"PriceWithTax":null}])";
  
  std::vector<PriceEntry> result = harness.testParseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String(""));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.0f);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

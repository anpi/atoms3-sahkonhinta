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

// Include ArduinoJson
#include <ArduinoJson.h>

#include "../src/PriceData.h"

// Test harness that exposes parseJsonToEntries for testing
class PriceMonitorTestHarness {
public:
  std::vector<PriceEntry> parseJsonToEntries(const String& json) {
    std::vector<PriceEntry> prices;
    
    DynamicJsonDocument doc(16384); // ~16KB for ~192 price entries
    DeserializationError error = deserializeJson(doc, json.c_str());
    
    if (error) {
      return prices; // empty vector indicates error
    }

    if (!doc.is<JsonArray>()) {
      return prices;
    }

    JsonArray priceArray = doc.as<JsonArray>();
    prices.reserve(priceArray.size());
    
    for (JsonObject obj : priceArray) {
      PriceEntry entry;
      entry.dateTime = String(obj["DateTime"].as<const char*>());
      entry.priceWithTax = obj["PriceWithTax"].as<float>();
      prices.push_back(entry);
    }
    
    return prices;
  }
};

// Test Suite: Core Functionality
TEST(ParseJsonToEntries, ValidJsonArray_BasicEntries) {
  PriceMonitorTestHarness harness;
  
  String json = R"([
    {"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.10},
    {"DateTime":"2025-11-18T10:15:00","PriceWithTax":0.11},
    {"DateTime":"2025-11-18T10:30:00","PriceWithTax":0.12}
  ])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(3));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00"));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.10f);
  EXPECT_EQ(result[1].dateTime, String("2025-11-18T10:15:00"));
  EXPECT_FLOAT_EQ(result[1].priceWithTax, 0.11f);
  EXPECT_EQ(result[2].dateTime, String("2025-11-18T10:30:00"));
  EXPECT_FLOAT_EQ(result[2].priceWithTax, 0.12f);
}

TEST(ParseJsonToEntries, ValidJsonArray_SingleEntry) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.15}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00"));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.15f);
}

TEST(ParseJsonToEntries, ValidJsonArray_EmptyArray) {
  PriceMonitorTestHarness harness;
  
  String json = "[]";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

TEST(ParseJsonToEntries, ValidJsonArray_LargeArray) {
  PriceMonitorTestHarness harness;
  
  // Build JSON with 50 entries
  std::string json = "[";
  for (int i = 0; i < 50; i++) {
    if (i > 0) json += ",";
    json += R"({"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.10})";
  }
  json += "]";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(String(json.c_str()));
  
  EXPECT_EQ(result.size(), static_cast<size_t>(50));
}

// Test Suite: Error Handling
TEST(ParseJsonToEntries, InvalidJson_MalformedSyntax) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.10,}])"; // trailing comma
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0)); // empty = error
}

TEST(ParseJsonToEntries, InvalidJson_NotArray_Object) {
  PriceMonitorTestHarness harness;
  
  String json = R"({"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.10})";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

TEST(ParseJsonToEntries, InvalidJson_NotArray_String) {
  PriceMonitorTestHarness harness;
  
  String json = R"("not an array")";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

TEST(ParseJsonToEntries, InvalidJson_CompletelyBroken) {
  PriceMonitorTestHarness harness;
  
  String json = "this is not json at all!@#$%";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

TEST(ParseJsonToEntries, InvalidJson_EmptyString) {
  PriceMonitorTestHarness harness;
  
  String json = "";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

TEST(ParseJsonToEntries, InvalidJson_UnclosedBracket) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.10})";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  EXPECT_EQ(result.size(), static_cast<size_t>(0));
}

// Test Suite: Field Validation
TEST(ParseJsonToEntries, MissingField_DateTime) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"PriceWithTax":0.10}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("")); // Empty string for missing field
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.10f);
}

TEST(ParseJsonToEntries, MissingField_PriceWithTax) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00"}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00"));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.0f); // Default value
}

TEST(ParseJsonToEntries, MissingFields_Both) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{}])"; // Empty object
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String(""));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.0f);
}

TEST(ParseJsonToEntries, ExtraFields_ShouldIgnore) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{
    "DateTime":"2025-11-18T10:00:00",
    "PriceWithTax":0.10,
    "ExtraField":"ignored",
    "AnotherField":123
  }])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00"));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.10f);
}

// Test Suite: Data Types
TEST(ParseJsonToEntries, WrongType_PriceAsString) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":"0.10"}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  // ArduinoJson will attempt conversion
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.10f);
}

TEST(ParseJsonToEntries, NegativePrice) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":-0.05}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, -0.05f);
}

TEST(ParseJsonToEntries, VeryLargePrice) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":999999.99}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_NEAR(result[0].priceWithTax, 999999.99f, 0.01f);
}

TEST(ParseJsonToEntries, ZeroPrice) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00","PriceWithTax":0.0}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.0f);
}

// Test Suite: DateTime Formats
TEST(ParseJsonToEntries, DateTimeFormat_WithMilliseconds) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00.000","PriceWithTax":0.10}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00.000"));
}

TEST(ParseJsonToEntries, DateTimeFormat_WithTimezone) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"2025-11-18T10:00:00+02:00","PriceWithTax":0.10}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00+02:00"));
}

TEST(ParseJsonToEntries, DateTimeFormat_Unusual) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":"18-11-2025 10:00","PriceWithTax":0.10}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("18-11-2025 10:00"));
}

// Test Suite: Edge Cases
TEST(ParseJsonToEntries, WhitespaceVariations) {
  PriceMonitorTestHarness harness;
  
  String json = R"(
    [
      {
        "DateTime"  :  "2025-11-18T10:00:00"  ,
        "PriceWithTax"  :  0.10
      }
    ]
  )";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String("2025-11-18T10:00:00"));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.10f);
}

TEST(ParseJsonToEntries, MultipleEntries_MixedValues) {
  PriceMonitorTestHarness harness;
  
  String json = R"([
    {"DateTime":"2025-11-18T00:00:00","PriceWithTax":0.05},
    {"DateTime":"2025-11-18T06:00:00","PriceWithTax":0.12},
    {"DateTime":"2025-11-18T12:00:00","PriceWithTax":0.18},
    {"DateTime":"2025-11-18T18:00:00","PriceWithTax":0.15},
    {"DateTime":"2025-11-18T23:45:00","PriceWithTax":0.08}
  ])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(5));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.05f);
  EXPECT_FLOAT_EQ(result[2].priceWithTax, 0.18f);
  EXPECT_FLOAT_EQ(result[4].priceWithTax, 0.08f);
}

TEST(ParseJsonToEntries, RealWorldExample_TypicalApiResponse) {
  PriceMonitorTestHarness harness;
  
  // Simulate typical API response with realistic data
  String json = R"([
    {"DateTime":"2025-11-18T00:00:00+02:00","PriceWithTax":0.0543},
    {"DateTime":"2025-11-18T00:15:00+02:00","PriceWithTax":0.0521},
    {"DateTime":"2025-11-18T00:30:00+02:00","PriceWithTax":0.0498},
    {"DateTime":"2025-11-18T00:45:00+02:00","PriceWithTax":0.0476}
  ])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(4));
  EXPECT_TRUE(result[0].dateTime.startsWith(String("2025-11-18T00:00")));
  EXPECT_NEAR(result[0].priceWithTax, 0.0543f, 0.0001f);
  EXPECT_NEAR(result[3].priceWithTax, 0.0476f, 0.0001f);
}

TEST(ParseJsonToEntries, NullValues_InObject) {
  PriceMonitorTestHarness harness;
  
  String json = R"([{"DateTime":null,"PriceWithTax":null}])";
  
  std::vector<PriceEntry> result = harness.parseJsonToEntries(json);
  
  ASSERT_EQ(result.size(), static_cast<size_t>(1));
  EXPECT_EQ(result[0].dateTime, String(""));
  EXPECT_FLOAT_EQ(result[0].priceWithTax, 0.0f);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

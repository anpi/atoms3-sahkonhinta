#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <string>

// Mock Arduino String class
class String {
private:
  std::string data;
public:
  String() = default;
  String(const char* s) : data(s ? s : "") {}
  String(const std::string& s) : data(s) {}
  
  size_t length() const { return data.length(); }
  const char* c_str() const { return data.c_str(); }
  
  String operator+(const String& other) const {
    return String((data + other.data).c_str());
  }
  
  String& operator+=(const String& other) {
    data += other.data;
    return *this;
  }
};

// Define before including headers
#define WString_h

#include "../../src/pricing/PriceData.h"
#include "../../src/display/IDisplayHardware.h"
#include "../../src/display/DisplayManager.cpp"

using ::testing::_;
using ::testing::InSequence;

// Mock display hardware using gMock
class MockDisplayHardware : public IDisplayHardware {
public:
  MOCK_METHOD(void, fillScreen, (uint16_t color), (override));
  MOCK_METHOD(void, fillCircle, (int x, int y, int radius, uint16_t color), (override));
  MOCK_METHOD(void, setTextColor, (uint16_t color), (override));
  MOCK_METHOD(void, setTextSize, (int size), (override));
  MOCK_METHOD(void, setCursor, (int x, int y), (override));
  MOCK_METHOD(void, print, (const String& text), (override));
  MOCK_METHOD(void, println, (const String& text), (override));
  MOCK_METHOD(void, setRotation, (int rotation), (override));
  MOCK_METHOD(void, setBrightness, (int level), (override));
};

// Test Suite: DisplayManager with gMock
TEST(DisplayManager, InitializeSetsRotationAndBrightness) {
  MockDisplayHardware mock;
  DisplayManager display(&mock);
  
  EXPECT_CALL(mock, setRotation(1)).Times(1);
  EXPECT_CALL(mock, setBrightness(1)).Times(1);
  
  display.initialize();
}

TEST(DisplayManager, ShowTextDisplaysTwoLines) {
  MockDisplayHardware mock;
  DisplayManager display(&mock);
  
  EXPECT_CALL(mock, fillScreen(0x0000));  // TFT_BLACK
  EXPECT_CALL(mock, setTextColor(0xFFFF));  // TFT_WHITE
  EXPECT_CALL(mock, setTextSize(1)).Times(::testing::AtLeast(1));
  EXPECT_CALL(mock, setCursor(4, 8));
  EXPECT_CALL(mock, println(::testing::_)).Times(2);  // Two lines
  EXPECT_CALL(mock, setCursor(4, 24));
  
  display.showText("Line 1", "Line 2");
}

TEST(DisplayManager, HighPriceShowsRedBackground) {
  MockDisplayHardware mock;
  DisplayManager display(&mock);
  
  PriceAnalysis analysis;
  analysis.valid = true;
  analysis.next90MinAvg = 0.20f;  // 20 cents - expensive
  analysis.cheapest90MinAvg = 0.05f;
  analysis.currentPeriodStartTime = "14:00";
  analysis.cheapest90MinTime = "02:00";
  analysis.lastFetchTime = "14:05";
  analysis.cheapestIsTomorrow = true;
  
  // Expect red background
  EXPECT_CALL(mock, fillScreen(0xC800)).Times(1);  // Red
  EXPECT_CALL(mock, setTextColor(_)).Times(::testing::AtLeast(1));
  EXPECT_CALL(mock, setTextSize(_)).Times(::testing::AtLeast(1));
  EXPECT_CALL(mock, setCursor(_, _)).Times(::testing::AtLeast(1));
  EXPECT_CALL(mock, print(_)).Times(::testing::AtLeast(1));
  
  display.showAnalysis(analysis);
}

TEST(DisplayManager, LowPriceShowsGreenBackground) {
  MockDisplayHardware mock;
  DisplayManager display(&mock);
  
  PriceAnalysis analysis;
  analysis.valid = true;
  analysis.next90MinAvg = 0.05f;  // 5 cents - cheap
  analysis.cheapest90MinAvg = 0.03f;
  analysis.currentPeriodStartTime = "14:00";
  analysis.cheapest90MinTime = "02:00";
  analysis.lastFetchTime = "14:05";
  analysis.cheapestIsTomorrow = false;
  
  // Expect green background
  EXPECT_CALL(mock, fillScreen(0x0320)).Times(1);  // Green
  EXPECT_CALL(mock, setTextColor(_)).Times(::testing::AtLeast(1));
  EXPECT_CALL(mock, setTextSize(_)).Times(::testing::AtLeast(1));
  EXPECT_CALL(mock, setCursor(_, _)).Times(::testing::AtLeast(1));
  EXPECT_CALL(mock, print(_)).Times(::testing::AtLeast(1));
  
  display.showAnalysis(analysis);
}

TEST(DisplayManager, TextIsCentered) {
  MockDisplayHardware mock;
  DisplayManager display(&mock);
  
  PriceAnalysis analysis;
  analysis.valid = true;
  analysis.next90MinAvg = 0.10f;  // 10 cents
  analysis.cheapest90MinAvg = 0.05f;
  analysis.currentPeriodStartTime = "14:00";
  analysis.cheapest90MinTime = "02:00";
  analysis.lastFetchTime = "14:05";
  analysis.cheapestIsTomorrow = false;
  
  // Verify centered text positioning
  EXPECT_CALL(mock, fillScreen(_)).Times(1);
  EXPECT_CALL(mock, setTextColor(_)).Times(::testing::AtLeast(1));
  EXPECT_CALL(mock, setTextSize(_)).Times(::testing::AtLeast(1));
  
  // Will be called 3 times: (4,4) for "Nyt", (centered, 24) for price, (4, 60) for "Halvin"
  // The centered call should have X >= 5
  EXPECT_CALL(mock, setCursor(_, _)).Times(::testing::AtLeast(3));
  EXPECT_CALL(mock, setCursor(::testing::Ge(5), 24)).Times(1);  // Price should be centered
  
  EXPECT_CALL(mock, print(_)).Times(::testing::AtLeast(1));
  
  display.showAnalysis(analysis);
}

TEST(DisplayManager, CallOrderIsCorrect) {
  MockDisplayHardware mock;
  DisplayManager display(&mock);
  
  PriceAnalysis analysis;
  analysis.valid = true;
  analysis.next90MinAvg = 0.10f;
  analysis.cheapest90MinAvg = 0.05f;
  analysis.currentPeriodStartTime = "14:00";
  analysis.cheapest90MinTime = "02:00";
  analysis.lastFetchTime = "14:05";
  analysis.cheapestIsTomorrow = false;
  
  // Verify fillScreen is called before any text operations
  {
    InSequence seq;
    EXPECT_CALL(mock, fillScreen(_)).Times(1);
    // Everything else comes after
    EXPECT_CALL(mock, setTextColor(_)).Times(::testing::AtLeast(1));
  }
  
  // Allow other calls without sequence constraints
  EXPECT_CALL(mock, setTextSize(_)).Times(::testing::AtLeast(1));
  EXPECT_CALL(mock, setCursor(_, _)).Times(::testing::AtLeast(1));
  EXPECT_CALL(mock, print(_)).Times(::testing::AtLeast(1));
  
  display.showAnalysis(analysis);
}

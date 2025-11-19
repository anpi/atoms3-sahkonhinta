#include <gtest/gtest.h>
#include <cstdint>

// Include the logic implementation directly (no hardware dependencies)
#include "../src/DisplayLogic.cpp"

// Test Suite: Display Color Scheme
TEST(DisplayLogic, ColorScheme_NegativePrice_ReturnsBlackWhite) {
  auto colors = DisplayLogic::determineColorScheme(-5.0f);
  
  EXPECT_EQ(colors.background, 0x0000);  // Black
  EXPECT_EQ(colors.text, 0xFFFF);        // White
}

TEST(DisplayLogic, ColorScheme_VeryLowPrice_ReturnsGreen) {
  auto colors = DisplayLogic::determineColorScheme(2.5f);
  
  EXPECT_EQ(colors.background, 0x0320);  // Green
  EXPECT_EQ(colors.text, 0xFFFF);        // White
}

TEST(DisplayLogic, ColorScheme_AtSevenPointNine_ReturnsGreen) {
  auto colors = DisplayLogic::determineColorScheme(7.9f);
  
  EXPECT_EQ(colors.background, 0x0320);  // Green
  EXPECT_EQ(colors.text, 0xFFFF);        // White
}

TEST(DisplayLogic, ColorScheme_MediumPrice_ReturnsYellow) {
  auto colors = DisplayLogic::determineColorScheme(10.0f);
  
  EXPECT_EQ(colors.background, 0xFC60);  // Yellow
  EXPECT_EQ(colors.text, 0x0000);        // Black
}

TEST(DisplayLogic, ColorScheme_AtFourteenPointNine_ReturnsYellow) {
  auto colors = DisplayLogic::determineColorScheme(14.9f);
  
  EXPECT_EQ(colors.background, 0xFC60);  // Yellow
  EXPECT_EQ(colors.text, 0x0000);        // Black
}

TEST(DisplayLogic, ColorScheme_HighPrice_ReturnsRed) {
  auto colors = DisplayLogic::determineColorScheme(20.0f);
  
  EXPECT_EQ(colors.background, 0xC800);  // Red
  EXPECT_EQ(colors.text, 0xFFFF);        // White
}

TEST(DisplayLogic, ColorScheme_AtFifteen_ReturnsRed) {
  auto colors = DisplayLogic::determineColorScheme(15.0f);
  
  EXPECT_EQ(colors.background, 0xC800);  // Red
  EXPECT_EQ(colors.text, 0xFFFF);        // White
}

TEST(DisplayLogic, ColorScheme_BoundaryEight_ReturnsYellow) {
  auto colors = DisplayLogic::determineColorScheme(8.0f);
  
  EXPECT_EQ(colors.background, 0xFC60);  // Yellow
  EXPECT_EQ(colors.text, 0x0000);        // Black
}

// Test Suite: Text Centering
TEST(DisplayLogic, CenterText_SmallText_CentersCorrectly) {
  int x = DisplayLogic::centerText(50);  // 50 pixels wide text on 128 pixel display
  
  EXPECT_EQ(x, 39);  // (128 - 50) / 2
}

TEST(DisplayLogic, CenterText_ExactlyHalfWidth_CentersAtZero) {
  int x = DisplayLogic::centerText(64, 128);
  
  EXPECT_EQ(x, 32);  // (128 - 64) / 2
}

TEST(DisplayLogic, CenterText_VeryWideText_ReturnsMinimumMargin) {
  int x = DisplayLogic::centerText(150, 128);
  
  EXPECT_EQ(x, 4);  // Safety: negative would become 4
}

TEST(DisplayLogic, CenterText_NegativeResult_ReturnsMinimumMargin) {
  int x = DisplayLogic::centerText(200, 128);
  
  EXPECT_EQ(x, 4);  // Safety check
}

TEST(DisplayLogic, CenterText_ZeroWidth_CentersAtMiddle) {
  int x = DisplayLogic::centerText(0, 128);
  
  EXPECT_EQ(x, 64);  // (128 - 0) / 2
}

TEST(DisplayLogic, CenterText_CustomDisplayWidth_CentersCorrectly) {
  int x = DisplayLogic::centerText(40, 100);
  
  EXPECT_EQ(x, 30);  // (100 - 40) / 2
}

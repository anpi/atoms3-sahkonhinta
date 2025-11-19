#include <gtest/gtest.h>

// Include the logic implementation directly (no hardware dependencies)
#include "../src/TimerLogic.cpp"

// Test Suite: Timer Calculation
TEST(TimerLogic, NextUpdate_BeforeFifteen_ReturnsSecondsUntilFifteen) {
  // At 10:30, next update is at :15 = 4.5 minutes = 270 seconds
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(10, 30);
  
  EXPECT_EQ(seconds, 270u);  // (15 - 10) * 60 - 30
}

TEST(TimerLogic, NextUpdate_AtZero_ReturnsSecondsUntilFifteen) {
  // At 00:00, next update is at :15 = 15 minutes = 900 seconds
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(0, 0);
  
  EXPECT_EQ(seconds, 900u);  // (15 - 0) * 60 - 0
}

TEST(TimerLogic, NextUpdate_BetweenFifteenAndThirty_ReturnsSecondsUntilThirty) {
  // At 20:45, next update is at :30 = 9.25 minutes = 555 seconds
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(20, 45);
  
  EXPECT_EQ(seconds, 555u);  // (30 - 20) * 60 - 45
}

TEST(TimerLogic, NextUpdate_BetweenThirtyAndFortyFive_ReturnsSecondsUntilFortyFive) {
  // At 35:20, next update is at :45 = 9.67 minutes = 580 seconds
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(35, 20);
  
  EXPECT_EQ(seconds, 580u);  // (45 - 35) * 60 - 20
}

TEST(TimerLogic, NextUpdate_AfterFortyFive_ReturnsSecondsUntilNextHour) {
  // At 50:10, next update is at :00 = 9.83 minutes = 590 seconds
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(50, 10);
  
  EXPECT_EQ(seconds, 590u);  // (60 - 50) * 60 - 10
}

TEST(TimerLogic, NextUpdate_AtExactQuarter_ReturnsAlmostFullInterval) {
  // At 15:00, next update is at :30 = 15 minutes = 900 seconds
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(15, 0);
  
  EXPECT_EQ(seconds, 900u);  // (30 - 15) * 60 - 0
}

TEST(TimerLogic, NextUpdate_OneSecondBeforeQuarter_ReturnsOneSecond) {
  // At 14:59, next update is at :15 = 1 second
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(14, 59);
  
  EXPECT_EQ(seconds, 1u);  // (15 - 14) * 60 - 59
}

TEST(TimerLogic, NextUpdate_AtThirty_ReturnsSecondsUntilFortyFive) {
  // At 30:00, next update is at :45 = 15 minutes = 900 seconds
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(30, 0);
  
  EXPECT_EQ(seconds, 900u);  // (45 - 30) * 60 - 0
}

TEST(TimerLogic, NextUpdate_AtFortyFive_ReturnsSecondsUntilNextHour) {
  // At 45:00, next update is at :00 = 15 minutes = 900 seconds
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(45, 0);
  
  EXPECT_EQ(seconds, 900u);  // (60 - 45) * 60 - 0
}

TEST(TimerLogic, NextUpdate_AtFiftyNine_ReturnsSecondsUntilNextHour) {
  // At 59:30, next update is at :00 = 30 seconds
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(59, 30);
  
  EXPECT_EQ(seconds, 30u);  // (60 - 59) * 60 - 30
}

TEST(TimerLogic, NextUpdate_EdgeCase_ThirtySeconds) {
  // At 14:30, next update is at :15 = 30 seconds
  uint64_t seconds = TimerLogic::getSecondsUntilNextUpdate(14, 30);
  
  EXPECT_EQ(seconds, 30u);  // (15 - 14) * 60 - 30
}

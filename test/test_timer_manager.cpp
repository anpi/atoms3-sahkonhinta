#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/ITimerHardware.h"

using ::testing::_;
using ::testing::Return;
using ::testing::InSequence;

// Define IRAM_ATTR as nothing for tests
#define IRAM_ATTR

// Mock volatile flags and ISR functions - define before including TimerManager.cpp
volatile bool buttonWakeFlag = false;
volatile bool timerFlag = false;
void buttonISR() {}
void timerISR() {}

// Mock for ITimerHardware
class MockTimerHardware : public ITimerHardware {
public:
  MOCK_METHOD(void*, timerBegin, (uint32_t frequency), (override));
  MOCK_METHOD(void, timerAttachInterrupt, (void* timer, void (*fn)()), (override));
  MOCK_METHOD(void, timerAlarm, (void* timer, uint64_t alarm_value, bool autoreload, uint64_t reload_count), (override));
  MOCK_METHOD(bool, getLocalTime, (struct tm* info), (override));
};

// Define guard to prevent TimerManager.cpp from redefining globals
#define TIMER_MANAGER_GLOBALS_DEFINED

#include "../src/TimerManager.h"
#include "../src/TimerManager.cpp"

TEST(TimerManager, SetupInitializesTimerWithCorrectFrequency) {
  MockTimerHardware mock;
  TimerManager timer(&mock);
  
  void* fakeTimer = reinterpret_cast<void*>(0x1234);
  
  EXPECT_CALL(mock, timerBegin(1000000))
    .Times(1)
    .WillOnce(Return(fakeTimer));
  EXPECT_CALL(mock, timerAttachInterrupt(fakeTimer, _))
    .Times(1);
  
  timer.setup();
}

TEST(TimerManager, ScheduleNextUpdate_BeforeFifteen_SetsCorrectAlarm) {
  MockTimerHardware mock;
  TimerManager timer(&mock);
  
  void* fakeTimer = reinterpret_cast<void*>(0x1234);
  EXPECT_CALL(mock, timerBegin(_)).WillOnce(Return(fakeTimer));
  EXPECT_CALL(mock, timerAttachInterrupt(_, _));
  timer.setup();
  
  // Mock time: 14:05:30 (minute=5, second=30)
  EXPECT_CALL(mock, getLocalTime(_))
    .WillOnce(::testing::Invoke([](struct tm* info) {
      info->tm_min = 5;
      info->tm_sec = 30;
      return true;
    }));
  
  // Next update at :15, so 9.5 minutes = 570 seconds
  EXPECT_CALL(mock, timerAlarm(fakeTimer, 570 * 1000000ULL, false, 0))
    .Times(1);
  
  timer.scheduleNextUpdate();
}

TEST(TimerManager, ScheduleNextUpdate_AtFifteen_SetsForThirty) {
  MockTimerHardware mock;
  TimerManager timer(&mock);
  
  void* fakeTimer = reinterpret_cast<void*>(0x1234);
  EXPECT_CALL(mock, timerBegin(_)).WillOnce(Return(fakeTimer));
  EXPECT_CALL(mock, timerAttachInterrupt(_, _));
  timer.setup();
  
  // Mock time: 14:15:00
  EXPECT_CALL(mock, getLocalTime(_))
    .WillOnce(::testing::Invoke([](struct tm* info) {
      info->tm_min = 15;
      info->tm_sec = 0;
      return true;
    }));
  
  // Next at :30, so 15 minutes = 900 seconds
  EXPECT_CALL(mock, timerAlarm(fakeTimer, 900 * 1000000ULL, false, 0))
    .Times(1);
  
  timer.scheduleNextUpdate();
}

TEST(TimerManager, ScheduleNextUpdate_AfterFortyFive_SetsForNextHour) {
  MockTimerHardware mock;
  TimerManager timer(&mock);
  
  void* fakeTimer = reinterpret_cast<void*>(0x1234);
  EXPECT_CALL(mock, timerBegin(_)).WillOnce(Return(fakeTimer));
  EXPECT_CALL(mock, timerAttachInterrupt(_, _));
  timer.setup();
  
  // Mock time: 14:50:00
  EXPECT_CALL(mock, getLocalTime(_))
    .WillOnce(::testing::Invoke([](struct tm* info) {
      info->tm_min = 50;
      info->tm_sec = 0;
      return true;
    }));
  
  // Next at :00, so 10 minutes = 600 seconds
  EXPECT_CALL(mock, timerAlarm(fakeTimer, 600 * 1000000ULL, false, 0))
    .Times(1);
  
  timer.scheduleNextUpdate();
}

TEST(TimerManager, ScheduleNextUpdate_TimeFailure_UsesFallback) {
  MockTimerHardware mock;
  TimerManager timer(&mock);
  
  void* fakeTimer = reinterpret_cast<void*>(0x1234);
  EXPECT_CALL(mock, timerBegin(_)).WillOnce(Return(fakeTimer));
  EXPECT_CALL(mock, timerAttachInterrupt(_, _));
  timer.setup();
  
  // getLocalTime fails
  EXPECT_CALL(mock, getLocalTime(_))
    .WillOnce(Return(false));
  
  // Should use 60 second fallback
  EXPECT_CALL(mock, timerAlarm(fakeTimer, 60 * 1000000ULL, false, 0))
    .Times(1);
  
  timer.scheduleNextUpdate();
}

TEST(TimerManager, SetupCallOrder_BeginBeforeAttach) {
  MockTimerHardware mock;
  TimerManager timer(&mock);
  
  void* fakeTimer = reinterpret_cast<void*>(0x1234);
  
  {
    InSequence seq;
    EXPECT_CALL(mock, timerBegin(1000000))
      .WillOnce(Return(fakeTimer));
    EXPECT_CALL(mock, timerAttachInterrupt(fakeTimer, _));
  }
  
  timer.setup();
}

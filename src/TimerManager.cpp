#include "TimerManager.h"
#ifndef TESTING
#include <Arduino.h>
#endif

volatile bool buttonWakeFlag = false;
volatile bool timerFlag = false;

void IRAM_ATTR buttonISR() {
  buttonWakeFlag = true;
}

void IRAM_ATTR timerISR() {
  timerFlag = true;
}

TimerManager::TimerManager(ITimerHardware* hardware) : hw(hardware), timer(nullptr) {}

void TimerManager::setup() {
  timer = hw->timerBegin(1000000);
  hw->timerAttachInterrupt(timer, &timerISR);
}

void TimerManager::scheduleNextUpdate() {
  struct tm timeinfo;
  uint64_t seconds = 60;  // Default fallback
  
  if (hw->getLocalTime(&timeinfo)) {
    seconds = getSecondsUntilNextUpdate(timeinfo.tm_min, timeinfo.tm_sec);
  }
  
  hw->timerAlarm(timer, seconds * 1000000, false, 0);
#ifndef TESTING
  Serial.printf("Timer set for %llu seconds\n", seconds);
#endif
}

bool TimerManager::wasTriggered() {
  if (timerFlag) {
    timerFlag = false;
    return true;
  }
  return false;
}

// Private logic method
uint64_t TimerManager::getSecondsUntilNextUpdate(int currentMinute, int currentSecond) {
  int nextMinute;
  if (currentMinute < 15) nextMinute = 15;
  else if (currentMinute < 30) nextMinute = 30;
  else if (currentMinute < 45) nextMinute = 45;
  else nextMinute = 60;
  
  int minutesUntilNext = nextMinute - currentMinute;
  if (minutesUntilNext <= 0) minutesUntilNext += 60;
  
  int secondsUntilNext = (minutesUntilNext * 60) - currentSecond;
  if (secondsUntilNext <= 0) secondsUntilNext = 60;
  
  return secondsUntilNext;
}

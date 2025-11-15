#include "TimerManager.h"

volatile bool buttonWakeFlag = false;
volatile bool timerFlag = false;

void IRAM_ATTR buttonISR() {
  buttonWakeFlag = true;
}

void IRAM_ATTR timerISR() {
  timerFlag = true;
}

uint64_t TimerManager::getSecondsUntilNextUpdate() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return 60;
  }
  
  int currentMinute = timeinfo.tm_min;
  int currentSecond = timeinfo.tm_sec;
  
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

void TimerManager::setup() {
  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &timerISR);
}

void TimerManager::scheduleNextUpdate() {
  uint64_t seconds = getSecondsUntilNextUpdate();
  timerAlarm(timer, seconds * 1000000, false, 0);
  Serial.printf("Timer set for %llu seconds\n", seconds);
}

bool TimerManager::wasTriggered() {
  if (timerFlag) {
    timerFlag = false;
    return true;
  }
  return false;
}

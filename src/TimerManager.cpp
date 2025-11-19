#include "TimerManager.h"
#include "TimerLogic.h"

volatile bool buttonWakeFlag = false;
volatile bool timerFlag = false;

void IRAM_ATTR buttonISR() {
  buttonWakeFlag = true;
}

void IRAM_ATTR timerISR() {
  timerFlag = true;
}

void TimerManager::setup() {
  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &timerISR);
}

void TimerManager::scheduleNextUpdate() {
  struct tm timeinfo;
  uint64_t seconds = 60;  // Default fallback
  
  if (getLocalTime(&timeinfo)) {
    seconds = TimerLogic::getSecondsUntilNextUpdate(timeinfo.tm_min, timeinfo.tm_sec);
  }
  
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

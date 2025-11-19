#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include "ITimerHardware.h"

#ifndef ARDUINO
#include <cstdint>
#include <ctime>
#else
#include <Arduino.h>
#include <time.h>
#endif

#ifdef ARDUINO
extern volatile bool buttonWakeFlag;
extern volatile bool timerFlag;

void IRAM_ATTR buttonISR();
void IRAM_ATTR timerISR();
#endif

class TimerManager {
private:
  ITimerHardware* hw;
  void* timer = nullptr;
  
  // Internal logic
  uint64_t getSecondsUntilNextUpdate(int currentMinute, int currentSecond);
  
public:
  TimerManager(ITimerHardware* hardware);
  
  void setup();
  void scheduleNextUpdate();
  bool wasTriggered();
};

#endif

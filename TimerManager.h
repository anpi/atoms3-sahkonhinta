#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <Arduino.h>
#include <time.h>

extern volatile bool buttonWakeFlag;
extern volatile bool timerFlag;

void IRAM_ATTR buttonISR();
void IRAM_ATTR timerISR();

class TimerManager {
private:
  hw_timer_t* timer = nullptr;
  uint64_t getSecondsUntilNextUpdate();
  
public:
  void setup();
  void scheduleNextUpdate();
  bool wasTriggered();
};

#endif

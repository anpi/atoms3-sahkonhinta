#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

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
#ifdef ARDUINO
private:
  hw_timer_t* timer = nullptr;
  
public:
  void setup();
  void scheduleNextUpdate();
  bool wasTriggered();
#else
public:
#endif
};

#endif

#ifndef M5_TIMER_HARDWARE_H
#define M5_TIMER_HARDWARE_H

#include "ITimerHardware.h"
#include <Arduino.h>
#include <time.h>

// M5AtomS3 concrete implementation of timer hardware
class M5TimerHardware : public ITimerHardware {
public:
  void* timerBegin(uint32_t frequency) override {
    return ::timerBegin(frequency);
  }
  
  void timerAttachInterrupt(void* timer, void (*fn)()) override {
    ::timerAttachInterrupt(static_cast<hw_timer_t*>(timer), fn);
  }
  
  void timerAlarm(void* timer, uint64_t alarm_value, bool autoreload, uint64_t reload_count) override {
    ::timerAlarm(static_cast<hw_timer_t*>(timer), alarm_value, autoreload, reload_count);
  }
  
  bool getLocalTime(struct tm* info) override {
    return ::getLocalTime(info);
  }
};

#endif

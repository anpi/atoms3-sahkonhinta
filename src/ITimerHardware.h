#ifndef ITIMER_HARDWARE_H
#define ITIMER_HARDWARE_H

#ifndef ARDUINO
#include <cstdint>
#include <ctime>
#else
#include <time.h>
#endif

// Hardware abstraction layer for timer operations
class ITimerHardware {
public:
  virtual ~ITimerHardware() = default;
  
  // Timer operations
  virtual void* timerBegin(uint32_t frequency) = 0;
  virtual void timerAttachInterrupt(void* timer, void (*fn)()) = 0;
  virtual void timerAlarm(void* timer, uint64_t alarm_value, bool autoreload, uint64_t reload_count) = 0;
  
  // Time operations
  virtual bool getLocalTime(struct tm* info) = 0;
};

#endif

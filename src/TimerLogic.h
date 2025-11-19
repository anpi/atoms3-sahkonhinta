#ifndef TIMER_LOGIC_H
#define TIMER_LOGIC_H

#include <cstdint>

// Pure logic for timer calculations, no hardware dependencies
class TimerLogic {
public:
  // Calculate seconds until next 15-minute interval (:00, :15, :30, :45)
  static uint64_t getSecondsUntilNextUpdate(int currentMinute, int currentSecond);
};

#endif // TIMER_LOGIC_H

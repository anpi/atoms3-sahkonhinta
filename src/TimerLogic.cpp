#include "TimerLogic.h"

uint64_t TimerLogic::getSecondsUntilNextUpdate(int currentMinute, int currentSecond) {
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

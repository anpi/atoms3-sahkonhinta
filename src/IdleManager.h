#ifndef IDLE_MANAGER_H
#define IDLE_MANAGER_H

class IdleManager {
public:
  void waitForInterrupt() {
    __asm__ __volatile__("waiti 0");
  }
};

#endif

#ifndef MOCK_DISPLAY_MANAGER_H
#define MOCK_DISPLAY_MANAGER_H

#include "../TestStringAdapter.h"
#include "../../src/PriceData.h"

class DisplayManager {
public:
  void initialize() {}
  void showText(const String&, const String& = "") {}
  void showLoadingIndicator() {}
  void showWifiIndicator() {}
  void showAnalysis(const PriceAnalysis&) {}
  void setBrightness(bool) {}
  void setBrightUntil(unsigned long) {}
  void updateBrightness(bool) {}
  bool isBright() const { return false; }
};

#endif

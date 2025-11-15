#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <WString.h>
#include "PriceData.h"

class DisplayManager {
private:
  bool bright = false;
  unsigned long brightUntil = 0;

public:
  void initialize();
  void showText(const String& l1, const String& l2 = "");
  void showLoadingIndicator();
  void showWifiIndicator();
  void showPrice(float eurPerKwh, const String& updateTime);
  void showAnalysis(const PriceAnalysis& analysis);
  void setBrightness(bool shouldBeBright);
  void setBrightUntil(unsigned long until);
  void updateBrightness(bool isFetching);
  bool isBright() const;
};

#endif

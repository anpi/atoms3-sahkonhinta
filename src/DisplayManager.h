#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#ifndef WString_h
#include <WString.h>
#endif
#include "PriceData.h"
#include "IDisplay.h"
#include "IDisplayHardware.h"

class DisplayManager : public IDisplay {
private:
  IDisplayHardware* hw;
  bool bright = false;
  unsigned long brightUntil = 0;
  
  // Internal logic methods
  struct ColorScheme {
    uint16_t background;
    uint16_t text;
  };
  
  ColorScheme determineColorScheme(float avgCents);
  int centerText(int textWidth, int displayWidth = 128);

public:
  DisplayManager(IDisplayHardware* hardware);
  
  void initialize() override;
  void showText(const String& l1, const String& l2 = "") override;
  void showLoadingIndicator() override;
  void showWifiIndicator() override;
  void showAnalysis(const PriceAnalysis& analysis) override;
  void setBrightness(bool shouldBeBright) override;
  void setBrightUntil(unsigned long until) override;
  void updateBrightness(bool isFetching) override;
  bool isBright() const override;
};

#endif

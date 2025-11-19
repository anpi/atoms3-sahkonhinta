#ifndef IDISPLAY_H
#define IDISPLAY_H

#ifndef WString_h
#include <WString.h>
#endif
#include "../pricing/PriceData.h"

/**
 * Interface for display operations.
 * Allows mocking in tests without hardware dependencies.
 */
class IDisplay {
public:
  virtual ~IDisplay() = default;
  
  virtual void initialize() = 0;
  virtual void showText(const String& l1, const String& l2 = "") = 0;
  virtual void showLoadingIndicator() = 0;
  virtual void showWifiIndicator() = 0;
  virtual void showAnalysis(const PriceAnalysis& analysis) = 0;
  virtual void setBrightness(bool shouldBeBright) = 0;
  virtual void setBrightUntil(unsigned long until) = 0;
  virtual void updateBrightness(bool isFetching) = 0;
  virtual bool isBright() const = 0;
};

#endif

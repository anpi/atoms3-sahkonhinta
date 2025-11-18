#ifndef MOCK_DISPLAY_H
#define MOCK_DISPLAY_H

#include "../../src/IDisplay.h"
#include "../TestStringAdapter.h"

/**
 * Mock implementation of IDisplay for testing.
 * Records calls for verification without hardware dependencies.
 */
class MockDisplay : public IDisplay {
public:
  String lastLine1;
  String lastLine2;
  bool initializeCalled = false;
  bool loadingIndicatorShown = false;
  bool wifiIndicatorShown = false;
  PriceAnalysis lastAnalysis;
  bool analysisShown = false;
  bool bright = false;
  unsigned long brightUntil = 0;

  void initialize() override {
    initializeCalled = true;
  }

  void showText(const String& l1, const String& l2 = "") override {
    lastLine1 = l1;
    lastLine2 = l2;
  }

  void showLoadingIndicator() override {
    loadingIndicatorShown = true;
  }

  void showWifiIndicator() override {
    wifiIndicatorShown = true;
  }

  void showAnalysis(const PriceAnalysis& analysis) override {
    lastAnalysis = analysis;
    analysisShown = true;
  }

  void setBrightness(bool shouldBeBright) override {
    bright = shouldBeBright;
  }

  void setBrightUntil(unsigned long until) override {
    brightUntil = until;
  }

  void updateBrightness(bool /* isFetching */) override {
    // Mock implementation - does nothing
  }

  bool isBright() const override {
    return bright;
  }
};

#endif

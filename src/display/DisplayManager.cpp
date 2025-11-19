#include "DisplayManager.h"
#ifndef TESTING
#include <Arduino.h>
#else
// Test stubs
unsigned long millis() { return 0; }
#endif

DisplayManager::DisplayManager(IDisplayHardware* hardware) : hw(hardware) {}

void DisplayManager::initialize() {
  hw->setRotation(1);
  hw->setBrightness(1);
}

void DisplayManager::showText(const String& l1, const String& l2) {
  hw->fillScreen(0x0000);  // TFT_BLACK
  hw->setTextColor(0xFFFF);  // TFT_WHITE
  hw->setTextSize(1);
  hw->setCursor(4, 8);
  hw->println(l1);
  if (l2.length()) {
    hw->setCursor(4, 24);
    hw->println(l2);
  }
}

void DisplayManager::showLoadingIndicator() {
  hw->fillCircle(120, 8, 6, 0xFC60);  // TFT_YELLOW
  hw->setTextColor(0x0000);  // TFT_BLACK
  hw->setTextSize(1);
  hw->setCursor(116, 4);
}

void DisplayManager::showWifiIndicator() {
  hw->fillCircle(120, 8, 6, 0x001F);  // TFT_BLUE
  hw->setTextColor(0xFFFF);  // TFT_WHITE
  hw->setTextSize(1);
  hw->setCursor(118, 4);
  hw->println("W");
}

void DisplayManager::showAnalysis(const PriceAnalysis& analysis) {
  if (!analysis.valid) return;

  float avgCents = analysis.next90MinAvg * 100.0f;
  float cheapestCents = analysis.cheapest90MinAvg * 100.0f;

  // Determine colors using testable helper
  ColorScheme colors = determineColorScheme(avgCents);

  hw->fillScreen(colors.background);
  hw->setTextColor(colors.text);
  
  // Top section: "Now HH:MM" label
  String nowLabel("Nyt ");
  nowLabel += analysis.currentPeriodStartTime;
  
  hw->setTextSize(1);
  hw->setTextColor(colors.text);
  hw->setCursor(4, 4);
  hw->print(nowLabel);
  
  // Next 90min price - centered between "Nyt" and "Halvin" labels
  hw->setTextSize(3);
  hw->setTextColor(colors.text);
  char buf[16];
  if (avgCents >= 0) {
    snprintf(buf, sizeof(buf), "%.1f c", avgCents);
    int textWidth = strlen(buf) * 18;  // Approx width for size 3
    int x = centerText(textWidth);
    hw->setCursor(x, 24);
    hw->print(buf);
  } else {
    hw->setCursor(30, 24);
    hw->print("N/A");
  }
  
  // Middle section: Cheapest period label
  hw->setTextSize(1);
  hw->setTextColor(colors.text);
  if (cheapestCents >= 0) {
    // Build the cheapest time string with optional (tom)
    String cheapestLabel("Halvin ");
    cheapestLabel += analysis.cheapest90MinTime;
    if (analysis.cheapestIsTomorrow) {
      cheapestLabel += " (huo)";
    }
    
    hw->setCursor(4, 68);
    hw->print(cheapestLabel);
    
    // Cheapest price - centered between "Halvin" and "Päivitetty" labels
    hw->setTextSize(2);
    hw->setTextColor(colors.text);
    snprintf(buf, sizeof(buf), "%.1f c", cheapestCents);
    int priceWidth = strlen(buf) * 12;  // Approx width for size 2
    int x = centerText(priceWidth);
    hw->setCursor(x, 88);
    hw->print(buf);
  } else {
    hw->setCursor(4, 88);
    hw->print("No data");
  }
  
  // Bottom: Update time
  hw->setTextSize(1);
  hw->setTextColor(colors.text);
  hw->setCursor(4, 116);
  hw->print("Päivitetty ");
  hw->print(analysis.lastFetchTime);
}

void DisplayManager::setBrightness(bool shouldBeBright) {
  if (shouldBeBright) {
    hw->setBrightness(255);
    bright = true;
  } else {
    hw->setBrightness(1);
    bright = false;
    brightUntil = 0;
  }
}

void DisplayManager::setBrightUntil(unsigned long until) {
  brightUntil = until;
}

void DisplayManager::updateBrightness(bool isFetching) {
  if (bright && !isFetching && brightUntil > 0 && millis() > brightUntil) {
    setBrightness(false);
  }
}

bool DisplayManager::isBright() const {
  return bright;
}

// Internal logic methods
DisplayManager::ColorScheme DisplayManager::determineColorScheme(float avgCents) {
  ColorScheme scheme;
  
  if (avgCents < 0) {
    // No data for next 90min - use neutral color
    scheme.background = 0x0000;  // TFT_BLACK
    scheme.text = 0xFFFF;  // TFT_WHITE
  } else if (avgCents < 8.0f) {
    scheme.background = 0x0320;  // Green
    scheme.text = 0xFFFF;  // TFT_WHITE
  } else if (avgCents < 15.0f) {
    scheme.background = 0xFC60;  // Yellow
    scheme.text = 0x0000;  // TFT_BLACK
  } else {
    scheme.background = 0xC800;  // Red
    scheme.text = 0xFFFF;  // TFT_WHITE
  }
  
  return scheme;
}

int DisplayManager::centerText(int textWidth, int displayWidth) {
  int x = (displayWidth - textWidth) / 2;
  return (x < 0) ? 4 : x;  // Safety check
}

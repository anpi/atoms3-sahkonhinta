#include "DisplayManager.h"
#include <M5AtomS3.h>

void DisplayManager::initialize() {
  AtomS3.Display.setRotation(1);
  AtomS3.Display.setBrightness(1);
}

void DisplayManager::showText(const String& l1, const String& l2) {
  AtomS3.Display.fillScreen(TFT_BLACK);
  AtomS3.Display.setTextColor(TFT_WHITE);
  AtomS3.Display.setTextSize(1);
  AtomS3.Display.setCursor(4, 8);
  AtomS3.Display.println(l1);
  if (l2.length()) {
    AtomS3.Display.setCursor(4, 24);
    AtomS3.Display.println(l2);
  }
}

void DisplayManager::showLoadingIndicator() {
  AtomS3.Display.fillCircle(120, 8, 6, TFT_YELLOW);
  AtomS3.Display.setTextColor(TFT_BLACK);
  AtomS3.Display.setTextSize(1);
  AtomS3.Display.setCursor(116, 4);
}

void DisplayManager::showWifiIndicator() {
  AtomS3.Display.fillCircle(120, 8, 6, TFT_BLUE);
  AtomS3.Display.setTextColor(TFT_WHITE);
  AtomS3.Display.setTextSize(1);
  AtomS3.Display.setCursor(118, 4);
  AtomS3.Display.println("W");
}

void DisplayManager::showAnalysis(const PriceAnalysis& analysis) {
  if (!analysis.valid) return;

  float avgCents = analysis.next90MinAvg * 100.0f;
  float cheapestCents = analysis.cheapest90MinAvg * 100.0f;

  // Determine background color based on next 90min average
  uint16_t bgColor, textColor;
  if (avgCents < 0) {
    // No data for next 90min - use neutral color
    bgColor = TFT_BLACK;
    textColor = TFT_WHITE;
  } else if (avgCents < 8.0f) {
    bgColor = 0x0320;  // Green
    textColor = TFT_WHITE;
  } else if (avgCents < 15.0f) {
    bgColor = 0xFC60;  // Yellow
    textColor = TFT_BLACK;
  } else {
    bgColor = 0xC800;  // Red
    textColor = TFT_WHITE;
  }

  AtomS3.Display.fillScreen(bgColor);
  AtomS3.Display.setTextColor(textColor);
  
  // Top section: "Now HH:MM" label
  String nowLabel = "Nyt " + analysis.currentPeriodStartTime;
  
  AtomS3.Display.setTextSize(1);
  AtomS3.Display.setTextColor(textColor);
  AtomS3.Display.setCursor(4, 4);
  AtomS3.Display.print(nowLabel);
  
  // Next 90min price - centered between "Nyt" and "Halvin" labels
  AtomS3.Display.setTextSize(3);
  AtomS3.Display.setTextColor(textColor);
  char buf[16];
  if (avgCents >= 0) {
    snprintf(buf, sizeof(buf), "%.1f c", avgCents);
    int textWidth = strlen(buf) * 18;  // Approx width for size 3
    int x = (128 - textWidth) / 2;
    if (x < 0) x = 4;  // Safety check
    AtomS3.Display.setCursor(x, 24);
    AtomS3.Display.print(buf);
  } else {
    AtomS3.Display.setCursor(30, 24);
    AtomS3.Display.print("N/A");
  }
  
  // Middle section: Cheapest period label
  AtomS3.Display.setTextSize(1);
  AtomS3.Display.setTextColor(textColor);
  if (cheapestCents >= 0) {
    // Build the cheapest time string with optional (tom)
    String cheapestLabel = "Halvin " + analysis.cheapest90MinTime;
    if (analysis.cheapestIsTomorrow) {
      cheapestLabel += " (huo)";
    }
    
    AtomS3.Display.setCursor(4, 68);
    AtomS3.Display.print(cheapestLabel);
    
    // Cheapest price - centered between "Halvin" and "Päivitetty" labels
    AtomS3.Display.setTextSize(2);
    AtomS3.Display.setTextColor(textColor);
    snprintf(buf, sizeof(buf), "%.1f c", cheapestCents);
    int priceWidth = strlen(buf) * 12;  // Approx width for size 2
    int x = (128 - priceWidth) / 2;
    if (x < 0) x = 4;  // Safety check
    AtomS3.Display.setCursor(x, 88);
    AtomS3.Display.print(buf);
  } else {
    AtomS3.Display.setCursor(4, 88);
    AtomS3.Display.print("No data");
  }
  
  // Bottom: Update time
  AtomS3.Display.setTextSize(1);
  AtomS3.Display.setTextColor(textColor);
  AtomS3.Display.setCursor(4, 116);
  AtomS3.Display.print("Päivitetty ");
  AtomS3.Display.print(analysis.lastFetchTime);
}

void DisplayManager::setBrightness(bool shouldBeBright) {
  if (shouldBeBright) {
    AtomS3.Display.setBrightness(255);
    bright = true;
  } else {
    AtomS3.Display.setBrightness(1);
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

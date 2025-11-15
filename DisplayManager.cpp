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
  AtomS3.Display.println("...");
}

void DisplayManager::showWifiIndicator() {
  AtomS3.Display.fillCircle(120, 8, 6, TFT_BLUE);
  AtomS3.Display.setTextColor(TFT_WHITE);
  AtomS3.Display.setTextSize(1);
  AtomS3.Display.setCursor(118, 4);
  AtomS3.Display.println("W");
}

void DisplayManager::showPrice(float eurPerKwh, const String& updateTime) {
  if (eurPerKwh < 0) return;

  float cents = eurPerKwh * 100.0f;
  char buf[32];
  snprintf(buf, sizeof(buf), "%.1f c/kWh", cents);

  uint16_t bgColor, textColor;
  if (cents < 8.0f) {
    bgColor = 0x0320;
    textColor = TFT_WHITE;
  } else if (cents < 15.0f) {
    bgColor = 0xFC60;
    textColor = TFT_BLACK;
  } else {
    bgColor = 0xC800;
    textColor = TFT_WHITE;
  }

  AtomS3.Display.fillScreen(bgColor);
  AtomS3.Display.setTextColor(textColor);
  AtomS3.Display.setTextSize(1);
  AtomS3.Display.setCursor(4, 4);
  AtomS3.Display.println("Spot price " + updateTime);
  AtomS3.Display.setTextSize(2);
  AtomS3.Display.setCursor(4, 24);
  AtomS3.Display.println(buf);
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

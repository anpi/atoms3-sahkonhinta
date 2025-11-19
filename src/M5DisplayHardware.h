#ifndef M5_DISPLAY_HARDWARE_H
#define M5_DISPLAY_HARDWARE_H

#include "IDisplayHardware.h"
#include <M5AtomS3.h>

// M5AtomS3 hardware implementation
class M5DisplayHardware : public IDisplayHardware {
public:
  void fillScreen(uint16_t color) override {
    AtomS3.Display.fillScreen(color);
  }
  
  void fillCircle(int x, int y, int radius, uint16_t color) override {
    AtomS3.Display.fillCircle(x, y, radius, color);
  }
  
  void setTextColor(uint16_t color) override {
    AtomS3.Display.setTextColor(color);
  }
  
  void setTextSize(int size) override {
    AtomS3.Display.setTextSize(size);
  }
  
  void setCursor(int x, int y) override {
    AtomS3.Display.setCursor(x, y);
  }
  
  void print(const String& text) override {
    AtomS3.Display.print(text);
  }
  
  void println(const String& text) override {
    AtomS3.Display.println(text);
  }
  
  void setRotation(int rotation) override {
    AtomS3.Display.setRotation(rotation);
  }
  
  void setBrightness(int level) override {
    AtomS3.Display.setBrightness(level);
  }
};

#endif // M5_DISPLAY_HARDWARE_H

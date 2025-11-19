#ifndef IDISPLAY_HARDWARE_H
#define IDISPLAY_HARDWARE_H

#ifndef WString_h
#include <WString.h>
#endif

// Hardware abstraction layer for display operations
class IDisplayHardware {
public:
  virtual ~IDisplayHardware() = default;
  
  // Screen operations
  virtual void fillScreen(uint16_t color) = 0;
  virtual void fillCircle(int x, int y, int radius, uint16_t color) = 0;
  
  // Text operations
  virtual void setTextColor(uint16_t color) = 0;
  virtual void setTextSize(int size) = 0;
  virtual void setCursor(int x, int y) = 0;
  virtual void print(const String& text) = 0;
  virtual void println(const String& text) = 0;
  
  // Display properties
  virtual void setRotation(int rotation) = 0;
  virtual void setBrightness(int level) = 0;
};

#endif // IDISPLAY_HARDWARE_H

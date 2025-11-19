#ifndef DISPLAY_LOGIC_H
#define DISPLAY_LOGIC_H

#include <cstdint>

// Pure logic for display calculations, no hardware dependencies
class DisplayLogic {
public:
  struct ColorScheme {
    uint16_t background;
    uint16_t text;
  };
  
  // Determine color scheme based on average price
  static ColorScheme determineColorScheme(float avgCents);
  
  // Calculate centered x position for text
  static int centerText(int textWidth, int displayWidth = 128);
};

#endif // DISPLAY_LOGIC_H

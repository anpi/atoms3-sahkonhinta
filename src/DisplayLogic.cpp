#include "DisplayLogic.h"

DisplayLogic::ColorScheme DisplayLogic::determineColorScheme(float avgCents) {
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

int DisplayLogic::centerText(int textWidth, int displayWidth) {
  int x = (displayWidth - textWidth) / 2;
  return (x < 0) ? 4 : x;  // Safety check
}

#include "LGFX_setup.h"

LGFX tft;

void setup_display() {
  tft.begin();
  tft.setRotation(0);
  tft.setBrightness(255);
}

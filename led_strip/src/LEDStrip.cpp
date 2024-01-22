// // LEDStrip.cpp
// #include "../lib/LEDStrip.h"


// LEDStrip::LEDStrip(int numLeds, uint8_t dataPin) : numLeds(numLeds), dataPin(dataPin) {
//   FastLED.addLeds<WS2812, 0, GBR>(leds, numLeds).setPin(dataPin);
// }

// void LEDStrip::update() {
//   FastLED.show();
// }

// void LEDStrip::setColor(CRGB color) {
//   for (int i = 0; i < numLeds; i++) {
//     leds[i] = color; 
//   }
// }
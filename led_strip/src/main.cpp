#include <Arduino.h>
#include <FastLED.h>

// main.cpp
#include "../lib/lightsaber.h"


// Instantiate the Lightsaber
Lightsaber lightsaber;

void setup() {
  // Initialize serial and any other setup code
  lightsaber.setLeds();
}

void loop() {
  // Use the lightsaber methods to control the LED strip
  // lightsaber.rainbowSweep();
  // lightsaber.rainbow();
  // lightsaber.setLedColor(6, CRGB(255, 255, 0));
  lightsaber.mixColor();
  // lightsaber.america();
}


// #define DATA_PIN 1
// #define NUM_LEDS 144
// CRGB leds[NUM_LEDS];
// elapsedMillis tic;
// int hue = 0;

// void setup() {
//   Serial.begin(115200);
//   Serial.println("FastLED Test");
//   FastLED.addLeds<WS2812, DATA_PIN, GBR>(leds, NUM_LEDS);
//   FastLED.setBrightness(50);
//   FastLED.clear();
// }

// void loop() {
//   if (tic >= 5) {
//     /* Shift prior colors up all the LEDs */
//     for (int i = 0; i < NUM_LEDS - 1; i++) {
//       leds[i] = leds[i + 1];
//     }
//     /* Set the last LED with a rainbow color */
//     leds[NUM_LEDS - 1] = CHSV(hue, 255, 255);
//     FastLED.show();
//     tic = 0;
//     hue += 5; // Increase hue to create a cycling rainbow effect
//     if (hue >= 256) {
//       hue = 0;
//     }
//   }
// }
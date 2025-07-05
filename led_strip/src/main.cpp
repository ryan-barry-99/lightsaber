#include <Arduino.h>
#include <FastLED.h>

// main.cpp
#include "lightsaber.h"


// Instantiate the Lightsaber
Lightsaber lightsaber;

void setup() {
  // Serial.begin(115200);
  pinMode(SELECTOR_BUTTON_PIN, INPUT);
  // Initialize serial and any other setup code
  lightsaber.setLeds();
//   FastLED.clear();       // Set all LEDs to black in memory
//   FastLED.show();        // Push that to the strip (turn them off physically)
}

void loop() {
  // Use the lightsaber methods to control the LED strip
  lightsaber.lightsaber_app();
}


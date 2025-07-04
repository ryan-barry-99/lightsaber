#include <Arduino.h>
#include <FastLED.h>

// main.cpp
#include "../include/lightsaber.h"


// Instantiate the Lightsaber
Lightsaber lightsaber;

void setup() {
  pinMode(2, INPUT_PULLDOWN);
  // Initialize serial and any other setup code
  lightsaber.setLeds();
}

void loop() {
  // Use the lightsaber methods to control the LED strip
  lightsaber.lightsaber_app();
}


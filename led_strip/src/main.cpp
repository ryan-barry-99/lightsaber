#include <Arduino.h>
#include <FastLED.h>

// main.cpp
#include "../lib/lightsaber.h"


// Instantiate the Lightsaber
Lightsaber lightsaber;

void setup() {
  pinMode(2, INPUT_PULLDOWN);
  // Initialize serial and any other setup code
  lightsaber.setLeds();
}

void loop() {
  // Use the lightsaber methods to control the LED strip
  
  switch(lightsaber.buttonSelect()){
    case 0:
    lightsaber.mixColor();
    break;
    case 1:
    lightsaber.rainbowSweep();
    break;
    case 2:
    lightsaber.rainbowSlowSweep();
    break;
    case 3:
    lightsaber.rainbowCycle();
    break;
    case 4:
    lightsaber.america();
    break;
  }
  // lightsaber.rainbowSweep();
  // lightsaber.rainbowSlowSweep();
  // lightsaber.rainbow();
  // lightsaber.setLedColor(6, CRGB(255, 255, 0));
  // lightsaber.mixColor();
  // lightsaber.america();
}


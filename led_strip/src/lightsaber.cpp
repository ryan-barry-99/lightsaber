// Lightsaber.cpp
#include "../lib/lightsaber.h"


Lightsaber::Lightsaber(){}

void Lightsaber::setLeds(){
  funcSelect = 0;
  hue = 0;
  FastLED.addLeds<WS2812, LED_PIN, GBR>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  FastLED.clear();
}

void Lightsaber::setLedColor(int led, CRGB color) {
  leds[led] = color;
  FastLED.show();
  delay(1);
}

void Lightsaber::rainbowSweep(){
  if (tic >= 5) {
    /* Shift prior colors up all the LEDs */
    for (int i = 0; i < NUM_LEDS - 1; i++) {
      leds[i] = leds[i + 1];
    }
    /* Set the last LED with a rainbow color */
    leds[NUM_LEDS - 1] = CHSV(hue, 255, 255);
    FastLED.show();
    tic = 0;
    hue += 1; // Increase hue to create a cycling rainbow effect
    if (hue >= 256) {
      hue = 0;
    }
  }
}

void Lightsaber::rainbowCycle() {
  static int hue = 0; // Declare hue as a static variable to retain its value across function calls
  CRGB color = CHSV(hue, 255, 255); // Create a color based on the current hue value
  fill_solid(leds, NUM_LEDS, color); // Set all LEDs to the same color
  FastLED.show();
  delay(50); // Adjust delay time to control speed of color change
  hue += 256 / NUM_LEDS; // Increment the hue value to move to the next color
  if (hue >= 256) {
    hue = 0;
  }
}

void Lightsaber::rainbowSlowSweep() {
  static int hue = 0; // Declare hue as a static variable to retain its value across function calls
  int increment = 256 / NUM_LEDS; // Calculate the increment value for each LED
  int currentIndex = 0; // Track the current index of the LED strip
  while (currentIndex < NUM_LEDS) {
    for (int i = 0; i < NUM_LEDS; i++) {
      int colorHue = hue + (i * increment); // Calculate the hue for each LED
      int colorSaturation = 255; // Set the saturation value for each LED
      int colorValue = 255; // Set the value (brightness) for each LED
      leds[i] = CHSV(colorHue, colorSaturation, colorValue); // Set the LED color based on the hue, saturation, and value
    }
    FastLED.show();
    delay(50); // Adjust delay time to control speed of color change
    currentIndex++;
  }
  hue += increment; // Increment the hue value to move to the next color
  if (hue >= 256) {
    hue = 0;
  }
}

void Lightsaber::rainbow(){
  int temp_hue = 0;
  int increment = 256 / NUM_LEDS;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(temp_hue, 255, 255);
    temp_hue += increment;
  }
  FastLED.show();
}

void Lightsaber::mixColor() {
  int red = analogRead(RED_DIAL) / 4;
  int green = analogRead(GREEN_DIAL) / 4;
  int blue = analogRead(BLUE_DIAL) / 4;
  if(red <= 5) red = 0;
  if(green <= 5) green = 0;
  if(blue <= 5) blue = 0;
  CRGB color = CRGB(red, green, blue);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
  delay(1);
}

void Lightsaber::america(){
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i % 3 == 0) {
      leds[i] = CRGB::Red;
    } else if (i % 3 == 1) {
      leds[i] = CRGB::White;
    } else {
      leds[i] = CRGB::Blue;
    }
  }
  FastLED.show();
}

int Lightsaber::buttonSelect(){
  mode_sel[0] = digitalRead(SELECTOR_BUTTON_PIN);
  if (mode_sel[0] != mode_sel[1]) {
    if (mode_sel[0] == HIGH) {
      funcSelect++;
      if (funcSelect == 2) {
        funcSelect = 0;
      }
    }
  }
  mode_sel[1] = mode_sel[0];
  return funcSelect;
}

void Lightsaber::detectIgnition(){
  ignition[0] = digitalRead(IGNITION_PIN);
  if (ignition[0] != ignition[1]) {
    if (ignition[0] == HIGH) {
      funcSelect++;
      if (funcSelect == 2) {
        funcSelect = 0;
      }
    }
  }
  ignition[1] = ignition[0];
}
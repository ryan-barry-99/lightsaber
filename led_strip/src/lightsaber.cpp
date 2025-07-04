// Lightsaber.cpp
#include "../include/lightsaber.h"


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
  if (tic >= 10*NUM_LEDS_PER_CM) {
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
    delay(25); // Adjust delay time to control speed of color change
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
      leds[i] = CRGB(0, 0, 255);
    } else if (i % 3 == 1) {
      leds[i] = CRGB(255, 255, 255);
    } else {
      leds[i] = CRGB(255, 0, 0);
    }
  }
  FastLED.show();
  delay(1);
}

void Lightsaber::flickeringFlame() {
  const int flickerDelay = 10; // Adjust the delay time for the flickering effect
  const int flickerChance = 40; // Adjust the chance of flickering (higher values increase flickering)

  for (int i = 0; i < NUM_LEDS; i++) {
    if (random(0, flickerChance) == 0) {
      // Flicker the LED with yellow color (255, 255, 0)
      leds[i] = CRGB(0, 255, 255);
    } else {
      // Set the LED to red color (255, 0, 0)
      leds[i] = CRGB(0, 0, 255);
    }
  }
  FastLED.show();
  delay(flickerDelay);
}

void Lightsaber::duelOfTheFates(){
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < NUM_LEDS/3) {
      leds[i] = CRGB(0, 0, 255);
    } else {
      leds[i] = CRGB(255, 0, 0);
    }
  }
  FastLED.show();
  delay(1);
}

void Lightsaber::duelOfTheFates2(){
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < NUM_LEDS/3) {
      leds[i] = CRGB(0, 0, 255);
    } else {
      leds[i] = CRGB(0, 255, 0);
    }
  }
  FastLED.show();
  delay(1);
}


int Lightsaber::buttonSelect(){
  mode_sel[0] = digitalRead(SELECTOR_BUTTON_PIN);
  if (mode_sel[0] != mode_sel[1]) {
    if (mode_sel[0] == HIGH) {
      funcSelect++;
      if (funcSelect == 6) {
        funcSelect = 0;
      }
    }
  }
  mode_sel[1] = mode_sel[0];
  return funcSelect;
}

void Lightsaber::detectIgnition(){
  ignition[0] = digitalRead(IGNITION_PIN);
  if (ignition[0] == HIGH) {
    lightsaber_on = true;
  }
  else {
    lightsaber_on = false;
  }

  if (ignition[0] != ignition[1]) {
    funcSelect = 0;
    if (ignition[0] == HIGH) {
      ignite_flag = true;
      extinguish_flag = false;
    }
    else {
      ignite_flag = false;
      extinguish_flag = true;
    }
  }
  ignition[1] = ignition[0];
  this->ignite();
}


void Lightsaber::ignite(){
  if (ignite_flag == true) {
    CRGB color = CRGB(0,0,0);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = color;
    }
    FastLED.show();
    int red = analogRead(RED_DIAL) / 4;
    int green = analogRead(GREEN_DIAL) / 4;
    int blue = analogRead(BLUE_DIAL) / 4;
    if(red <= 5) red = 0;
    if(green <= 5) green = 0;
    if(blue <= 5) blue = 0;
    color = CRGB(red, green, blue);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = color;
      delay(150.0/NUM_LEDS_PER_METER);
      FastLED.show();
    }
    delay(1);
    ignite_flag = false;
  }
}

void Lightsaber::extinguish(){
  CRGB color = CRGB(0,0,0);
  if (extinguish_flag == true){
    for (int i = NUM_LEDS - 1; i >= 0; i--) {
      leds[i] = color;
      delay(150.0/NUM_LEDS_PER_METER);
      FastLED.show();
    }
    extinguish_flag = false;
  }
  else{
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = color;
    }
      FastLED.show();
  }
  delay(1);
}

void Lightsaber::lightsaber_app(){
  this->detectIgnition();
  if (lightsaber_on == true) {
    switch(this->buttonSelect()){
      case 0:
      this->mixColor();
      break;
      case 1:
      this->rainbowSweep();
      break;
      case 2:
      this->flickeringFlame();
      break;
      case 3:
      this->america();
      break;
      case 4:
      this->duelOfTheFates();
      break;
      case 5:
      this->duelOfTheFates2();
      break;
    }
  }
  else{
    this->extinguish();
  }
}
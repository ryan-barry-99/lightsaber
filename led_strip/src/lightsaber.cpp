// Lightsaber.cpp
#include "lightsaber.h"


Lightsaber::Lightsaber() : tic(0){}

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
  if (millis() - this->tic >= 1000) {
        this->tic = millis();
        // do stuff
    }
  if (this->tic >= 10*NUM_LEDS_PER_CM) {
    /* Shift prior colors up all the LEDs */
    for (int16_t i = 0; i < NUM_LEDS - 1; i++) {
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
  int16_t increment = 256 / NUM_LEDS; // Calculate the increment value for each LED
  uint16_t currentIndex = 0; // Track the current index of the LED strip
  while (currentIndex < NUM_LEDS) {
    for (int16_t i = 0; i < NUM_LEDS; i++) {
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
  int16_t increment = 256 / NUM_LEDS;
  
  for (int16_t i = 0; i < NUM_LEDS; i++) {
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
  for (int16_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
  delay(1);
}

void Lightsaber::america() {
  static uint8_t offset = 0;

  const CRGB red   = CRGB(0, 0, 255);   // GBR
  const CRGB white = CRGB(255, 255, 255);
  const CRGB blue  = CRGB(255, 0, 0);
  const int flickerChance = 30;

  const int blueWidth = NUM_LEDS / 2 - 19;
  const int stripeWidth = 7;  // each red or white stripe is 10 LEDs wide

  for (int16_t i = 0; i < NUM_LEDS; i++) {
    int pos = (i + offset) % NUM_LEDS;

    if (pos < blueWidth) {
      // Blue section with twinkling white stars
      leds[i] = (random(0, flickerChance) == 0) ? white : blue;
    } else {
      // Alternating red and white stripes
      int stripeIndex = (pos - blueWidth) / stripeWidth;
      leds[i] = (stripeIndex % 2 == 0) ? red : white;
    }
  }

  FastLED.show();
  delay(15);  // control speed of animation
  offset = (offset + 1) % NUM_LEDS;
}

void Lightsaber::lavaFlow() {
  static uint16_t noiseOffset = 0;
  static uint32_t lastBurst = 0;

  const CRGB deepRed   = CRGB(0, 0, 255);
  const CRGB hotRed    = CRGB(0, 30, 255);
  const CRGB orange    = CRGB(0, 80, 200);
  const CRGB yellow    = CRGB(0, 140, 255);
  const CRGB whiteHot  = CRGB(15, 150, 255);
  const CRGB cooled    = CRGB(0, 0, 20);   // Very dim red (almost black)

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t baseHeat = inoise8(i * 10, noiseOffset);
    uint8_t flicker = baseHeat + random8(30); // more chaotic

    CRGB color;

    if (flicker < 60) {
      // Cooling lava crust
      color = cooled;
    }
    else if (flicker < 100) {
      color = blend(deepRed, hotRed, (flicker - 60) * 6); // scale up for full 0–255
    }
    else if (flicker < 160) {
      color = blend(hotRed, orange, (flicker - 100) * 4);
    }
    else if (flicker < 210) {
      color = blend(orange, yellow, (flicker - 160) * 5);
    }
    else {
      color = whiteHot; // rare burst of extreme heat
    }

    leds[i] = color;
  }

  // Optional: small bursts of intense brightness
  if (millis() - lastBurst > 1600 && random8() < 30) {
    uint16_t burstCenter = random16(NUM_LEDS - 3);
    for (uint8_t j = 0; j < 3; j++) {
      leds[burstCenter + j] = whiteHot;
    }
    lastBurst = millis();
  }

  FastLED.show();
  noiseOffset += 1;
  delay(20);
}



void Lightsaber::flickeringFlame() {
  const int flickerDelay = 10; // Adjust the delay time for the flickering effect
  const int flickerChance = 40; // Adjust the chance of flickering (higher values increase flickering)

  for (int16_t i = 0; i < NUM_LEDS; i++) {
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

void Lightsaber::duelOfTheFates() {
  const int gradientStart = NUM_LEDS/3;
  const int gradientEnd   = NUM_LEDS / 3 + 10;

  for (int16_t i = 0; i < NUM_LEDS; i++) {
    if (i < gradientStart) {
      leds[i] = CRGB(0, 0, 255); // blue
    }
    else if (i > gradientEnd) {
      leds[i] = CRGB(255, 0, 0); // red
    }
    else {
      // Blend based on position within gradient range
      uint8_t blendAmount = map(i, gradientStart, gradientEnd, 0, 255);
      leds[i] = blend(CRGB(0, 0, 255), CRGB(255, 0, 0), blendAmount); // blue to red
    }
  }

  FastLED.show();
}

void Lightsaber::duelOfTheFates2() {
  const int gradientStart = NUM_LEDS / 3;
  const int gradientEnd   = NUM_LEDS / 3 + 10;

  for (int16_t i = 0; i < NUM_LEDS; i++) {
    if (i < gradientStart) {
      leds[i] = CRGB(0, 0, 255); // green (GBR)
    }
    else if (i > gradientEnd) {
      leds[i] = CRGB(0, 255, 0); // red (GBR)
    }
    else {
      uint8_t blendAmount = map(i, gradientStart, gradientEnd, 0, 255);
      leds[i] = blend(CRGB(0, 0, 255), CRGB(0, 255, 0), blendAmount); // green → red (GBR)
    }
  }

  FastLED.show();
}



void Lightsaber::deadSaber() {
  static uint32_t lastUpdate = 0;
  static const uint8_t baseDimMin = 3;   // Almost off
  static const uint8_t baseDimMax = 10;   // Very dim red
  static const uint8_t flickerChance = 5;  // Low chance of flicker
  static const uint8_t flareChance = 1;     // Very rare flare
  static uint8_t cooldown[NUM_LEDS];

  uint32_t now = millis();
  if (now - lastUpdate < 40) return;  // ~25 FPS update rate
  lastUpdate = now;

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    if (cooldown[i] > 0) cooldown[i]--;

    uint8_t roll = random8(100);
    if (roll < flareChance && cooldown[i] == 0) {
      // Very rare flare
      leds[i] = CRGB(0, 0, 255); // full red (GBR)
      cooldown[i] = 10;
    } else if (roll < flickerChance && cooldown[i] == 0) {
      // Mid brightness flicker
      leds[i] = CRGB(0, 0, random8(30, 80));
      cooldown[i] = 6;
    } else if (cooldown[i] == 0) {
      // Mostly dim and uneven
      leds[i] = CRGB(0, 0, random8(baseDimMin, baseDimMax));
    }
  }

  FastLED.show();
}








int Lightsaber::buttonSelect() {
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 15;  // in milliseconds

  int reading = digitalRead(SELECTOR_BUTTON_PIN);

  if (reading != mode_sel[1]) {
    // reset the debounce timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != mode_sel[0]) {
      mode_sel[0] = reading;
      if (mode_sel[0] == HIGH) {
        funcSelect++;
        if (funcSelect == 8) {
          funcSelect = 0;
        }
      }
    }
  }

  mode_sel[1] = reading;
  return funcSelect;
}


void Lightsaber::detectIgnition() {
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 40;  // ms

  int currentReading = digitalRead(IGNITION_PIN);

  if (currentReading != ignition[1]) {
    // Reset debounce timer if there's a change
    lastDebounceTime = millis();
  }

  // Only consider it a stable change if it's lasted debounceDelay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentReading != ignition[0]) {
      ignition[0] = currentReading;

      if (ignition[0] == HIGH && lightsaber_state == LIGHTSABER_OFF) {
        lightsaber_state = LIGHTSABER_IGNITE;
      } else if (ignition[0] == LOW && lightsaber_state == LIGHTSABER_ON) {
        lightsaber_state = LIGHTSABER_EXTINGUISH;
      }
    }
  }

  ignition[1] = currentReading;

  // Handle state transitions
  if (lightsaber_state == LIGHTSABER_IGNITE) {
    this->ignite();
    return;
  } else if (lightsaber_state == LIGHTSABER_EXTINGUISH) {
    this->extinguish();
    return;
  }
}






void Lightsaber::ignite(){
  // if (ignite_flag == true) {
    CRGB color = CRGB(0,0,0);
    for (int16_t i = 0; i < NUM_LEDS; i++) {
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
    for (int16_t i = 0; i < NUM_LEDS; i++) {
      leds[i] = color;
      delay(0.01);
      FastLED.show();
    }
    // delay(1);
    lightsaber_state = LIGHTSABER_ON;

    // ignite_flag = false;
    // lightsaber_on = true;
    // extinguish_flag = true;
  // }    

}

void Lightsaber::extinguish(){
  CRGB color = CRGB(0,0,0);
  // if (extinguish_flag == true){
    for (int16_t i = NUM_LEDS - 1; i >= 0; i--) {
      leds[i] = color;
      delay(0.01);
      FastLED.show();
    }
  lightsaber_state = LIGHTSABER_OFF;
  funcSelect = 0;
}

void Lightsaber::lightsaber_app() {
  this->detectIgnition();

  // if (ignite_flag) {
  //   this->ignite();
  //   return;
  // }

  // if (extinguish_flag) {
  //   this->extinguish();
  //   return;
  // }

  if (lightsaber_state == LIGHTSABER_ON) {
    switch (this->buttonSelect()) {
      case 0: this->mixColor(); break;
      case 1: this->rainbowSweep(); break;
      case 2: this->flickeringFlame(); break;
      case 3: this->lavaFlow(); break;
      case 4: this->america(); break;
      case 5: this->duelOfTheFates(); break;
      case 6: this->duelOfTheFates2(); break;
      case 7: this->deadSaber(); break;
    }
  }
}


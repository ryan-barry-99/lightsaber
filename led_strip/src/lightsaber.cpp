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
  if(red <= 10) red = 0;
  if(green <= 10) green = 0;
  if(blue <= 10) blue = 0;
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
  static uint32_t burstStartTime = 0;
  static uint8_t burstState = 0;  // 0 = idle, 1 = blinking, 2 = solid, 3 = dying flicker, 4 = cooldown
  static uint8_t blinkCount = 0;
  static uint8_t targetBlinks = 0;
  static uint32_t solidDuration = 0;
  static const uint16_t updateDelay = 40;

  static const uint8_t baseDimMin = 3;
  static const uint8_t baseDimMax = 10;
  static const uint8_t flickerChance = 5;
  static const uint8_t flareChance = 1;
  static const uint8_t burstChance = 2; // More frequent than before
  static const uint16_t blinkDuration = 100;
  static const uint16_t burstCooldownMin = 4000;
  static const uint16_t burstCooldownMax = 10000;
  static uint32_t nextBurstDelay = 6000;

  static uint8_t cooldown[NUM_LEDS];
  uint32_t now = millis();

  if (now - lastUpdate < updateDelay) return;
  lastUpdate = now;

  // Handle the burst sequence
  switch (burstState) {
    case 1: { // Blinking
      bool onPhase = ((now - burstStartTime) / blinkDuration) % 2 == 0;
      fill_solid(leds, NUM_LEDS, onPhase ? CRGB(0, 0, 255) : CRGB::Black);
      FastLED.show();

      if ((now - burstStartTime) > blinkDuration * 2 * targetBlinks) {
        burstState = 2;
        burstStartTime = now;
        solidDuration = random(250, 2500);  // 0.5s to 2.5s
      }
      return;
    }
    case 2: { // Solid red
      fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255));
      FastLED.show();

      if ((now - burstStartTime) > solidDuration) {
        burstState = 3; // Move to dying flicker
        burstStartTime = now;
        blinkCount = 0;
      }
      return;
    }
    case 3: { // Dying flicker (quick 2–3 flickers as it shuts off)
      bool on = ((now - burstStartTime) / blinkDuration) % 2 == 0;
      fill_solid(leds, NUM_LEDS, on ? CRGB(0, 0, 255) : CRGB::Black);
      FastLED.show();

      if ((now - burstStartTime) > blinkDuration * 2 * 2) {
        burstState = 4;
        burstStartTime = now;
        nextBurstDelay = random(burstCooldownMin, burstCooldownMax);
      }
      return;
    }
    case 4: { // Cooldown
      if (now - burstStartTime > nextBurstDelay) {
        burstState = 0;
      }
      break;
    }
  }

  // Start a new burst randomly
  if (burstState == 0 && random(1000) < burstChance) {
    burstState = 1;
    burstStartTime = now;
    targetBlinks = random(0, 6);  // 2 to 5 blinks
    return;
  }

  // Normal flickering
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    if (cooldown[i] > 0) cooldown[i]--;

    uint8_t roll = random8(100);
    if (roll < flareChance && cooldown[i] == 0) {
      leds[i] = CRGB(0, 0, 255); // red flare
      cooldown[i] = 10;
    } else if (roll < flickerChance && cooldown[i] == 0) {
      leds[i] = CRGB(0, 0, random8(30, 80));
      cooldown[i] = 6;
    } else if (cooldown[i] == 0) {
      leds[i] = CRGB(0, 0, random8(baseDimMin, baseDimMax));
    }
  }

  FastLED.show();
}



void Lightsaber::nebulaBlade() {
  static uint16_t timebase = 0;
  static uint8_t hueShift = 0;

  // Soft global brightness pulse (like breathing)
  uint8_t pulse = beatsin8(3, 100, 255); // 3 bpm swell, brightness from 100–255

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    // Each LED has a slight hue offset from the base
    uint8_t hue = hueShift + sin8(i * 5 + timebase / 2);
    CRGB color = CHSV(hue, 200, pulse);

    // Occasional star twinkle layer
    if (random8() < 2) {
      color += CRGB(20, 20, 20); // subtle sparkle
    }

    leds[i] = color;
  }

  FastLED.show();

  // Advance wave pattern and color shift
  timebase += 2;
  hueShift += 1;
}


void Lightsaber::nebulaDrift() {
  const uint8_t numZones = 16;
  static struct {
    int center;
    int width;
    uint8_t hue;
    int8_t driftSpeed;
    uint8_t brightnessPhase;
    int8_t brightnessDelta;
  } zones[numZones];

  static bool initialized = false;
  if (!initialized) {
    for (uint8_t i = 0; i < numZones; i++) {
      zones[i].center = random(NUM_LEDS);
      zones[i].width = random(4, 8);
      zones[i].hue = random8();
      zones[i].driftSpeed = random(1, 8) * (random(0, 2) ? 1 : -1);
      zones[i].brightnessPhase = random8();
      zones[i].brightnessDelta = random(1, 3);
    }
    initialized = true;
  }

  // Fade existing colors to prevent washout
  fadeToBlackBy(leds, NUM_LEDS, 10);

  for (uint8_t z = 0; z < numZones; z++) {
    // Update drift position
    zones[z].center += zones[z].driftSpeed;
    if (zones[z].center < -zones[z].width) zones[z].center = NUM_LEDS + zones[z].width;
    if (zones[z].center > NUM_LEDS + zones[z].width) zones[z].center = -zones[z].width;

    // Update brightness pulse
    zones[z].brightnessPhase += zones[z].brightnessDelta;
    uint8_t baseBright = (sin8(zones[z].brightnessPhase) + 64) >> 1; // ~0–127

    for (int i = -zones[z].width; i <= zones[z].width; i++) {
      int pos = zones[z].center + i;
      if (pos < 0 || pos >= NUM_LEDS) continue;

      uint8_t falloff = 255 - abs(i) * (255 / zones[z].width);
      uint8_t brightness = scale8(baseBright, falloff);
      CRGB nebulaColor = CHSV(zones[z].hue, 150, brightness);  // Slightly desaturated

      leds[pos] += nebulaColor;  // Add glow
    }
  }

  FastLED.show();
  delay(30);
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
        if (funcSelect == 10) {
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
      case 2: this->nebulaBlade(); break;
      case 3: this->nebulaDrift(); break;
      case 4: this->flickeringFlame(); break;
      case 5: this->lavaFlow(); break;
      case 6: this->america(); break;
      case 7: this->duelOfTheFates(); break;
      case 8: this->duelOfTheFates2(); break;
      case 9: this->deadSaber(); break;
    }
  }
}


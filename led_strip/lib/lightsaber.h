// Lightsaber.h
#ifndef LIGHTSABER_H
#define LIGHTSABER_H

#include <FastLED.h>

#define NUM_LEDS 66
#define LED_PIN 1
#define RED_DIAL A0
#define GREEN_DIAL A1
#define BLUE_DIAL A2

class Lightsaber {
  public:
    Lightsaber();
    void setLeds();
    void setLedColor(int led, CRGB color);
    void rainbowSweep();
    void rainbow();
    void mixColor();
    void america();
  private:
    CRGB leds[NUM_LEDS];
    int numLeds;
    uint8_t dataPin;
    elapsedMillis tic;
    int hue = 0;
};

#endif
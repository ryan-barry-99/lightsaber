// Lightsaber.h
#ifndef LIGHTSABER_H
#define LIGHTSABER_H

#include <FastLED.h>

#define LENGTH 1 // 1M strip
#define LENGTH_CM LENGTH * 100
#define NUM_LEDS_PER_METER 144
#define NUM_LEDS_PER_CM NUM_LEDS_PER_METER / 100
#define NUM_LEDS LENGTH * NUM_LEDS_PER_METER
#define LED_PIN 1
#define SELECTOR_BUTTON_PIN 11
#define IGNITION_PIN 2
#define RED_DIAL A0
#define GREEN_DIAL A1
#define BLUE_DIAL A2

class Lightsaber {
  public:
    Lightsaber();
    void detectIgnition();
    void setLeds();
    void setLedColor(int led, CRGB color);
    void mixColor();
    void rainbowSweep();
    int rgb2hue(const CRGB& color);
    void rainbowCycle();
    void rainbowSlowSweep();
    void rainbow();
    void america();
    void flickeringFlame();
    void duelOfTheFates();
    void duelOfTheFates2();
    int buttonSelect();
    void ignite();
    void extinguish();
    void lightsaber_app();
    int funcSelect;
  private:
    CRGB leds[NUM_LEDS];
    int numLeds;
    uint8_t dataPin;
    elapsedMillis tic;
    int hue;
    int mode_sel[2] = {LOW, HIGH};
    int ignition[2] = {LOW, HIGH};
    bool lightsaber_on = false;
    bool ignite_flag = false;
    bool extinguish_flag = false;
};

#endif
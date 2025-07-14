#pragma once

struct SimpleRGB {
    int r, g, b;
    SimpleRGB(int r_ = 0, int g_ = 0, int b_ = 0) : r(r_), g(g_), b(b_) {}
};

class SaberSoundEngine {
public:
    // ledsPtr should point to at least one SimpleRGB object
    explicit SaberSoundEngine(double sampleRate, SimpleRGB* ledsPtr);

    void setAccel(double a);
    void setColor(const SimpleRGB& color);
    void nextSample(float& left, float& right);

private:
    double sampleRate;
    double phase, modPhase;
    double baseFreq;
    double vibratoDepth;
    double accel;
    SimpleRGB* leds; // Pointer to external LED data (for dynamic color)
};
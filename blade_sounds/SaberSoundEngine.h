#pragma once
#include <vector>

struct SimpleRGB {
    int r, g, b;
    SimpleRGB(int r_ = 0, int g_ = 0, int b_ = 0) : r(r_), g(g_), b(b_) {}
};

enum SaberSoundState {
    SABER_IGNITION,
    SABER_HUM
};

class SaberSoundEngine {
public:
    explicit SaberSoundEngine(double sampleRate);

    void setColor(const SimpleRGB& color);

    void nextSample(float& left, float& right);

    void startIgnition();

    SaberSoundState getState() const { return state; }
    SimpleRGB getCurrentColor() const { return color; }

private:
    double sampleRate;
    double phase, modPhase;
    double baseFreq;
    double vibratoDepth;
    SimpleRGB color;

    SaberSoundState state;
    std::vector<float> ignitionPCM;
    size_t ignitionPos;

    void generateIgnitionPCM(const SimpleRGB& color);
};
#include "SaberSoundEngine.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

constexpr double PI = 3.14159265358979323846;
constexpr double VOLUME = 10;        // Output volume
constexpr double BASE_FREQ = 65.0;    // Low end of hum (Hz)
constexpr double FREQ_RANGE = 190.0;  // How much color raises pitch (Hz)
constexpr double BASE_VIBRATO = 0.18; // Vibrato depth (Hz)
constexpr double SWING_PITCH_BEND = 90.0; // How much a swing raises pitch (Hz)

SaberSoundEngine::SaberSoundEngine(double sr, SimpleRGB* ledsPtr)
    : sampleRate(sr)
    , phase(0.0)
    , modPhase(0.0)
    , baseFreq(BASE_FREQ)
    , vibratoDepth(BASE_VIBRATO)
    , accel(0.0)
    , leds(ledsPtr)
{}

void SaberSoundEngine::setAccel(double a) { accel = a; }

void SaberSoundEngine::setColor(const SimpleRGB& color) {
    leds[0] = color;
}

void SaberSoundEngine::nextSample(float& left, float& right) {
    // Normalize color
    double r = leds[0].r / 255.0;
    double g = leds[0].g / 255.0;
    double b = leds[0].b / 255.0;
    double total = r + g + b;
    if (total < 0.01) total = 1.0;
    r /= total; g /= total; b /= total;

    // Map color to frequency (pitch)
    double colorFreq = BASE_FREQ + r*40.0 + g*65.0 + b*FREQ_RANGE;
    // Vibrato rate also changes by color
    double colorVibrato = 1.5 + r*2.0 + g*3.5 + b*6.2; // Tune as needed
    // Add swing (accel) effect
    double swingBend = SWING_PITCH_BEND * std::clamp(accel, 0.0, 1.0);

    double vibrato = vibratoDepth * sin(modPhase);
    double freq = colorFreq + swingBend + vibrato;

    double sample = sin(phase) * VOLUME;
    sample += 0.17 * sin(phase * 2.0) * VOLUME;

    left = right = (float)sample;

    phase    += 2.0 * PI * freq / sampleRate;
    if (phase > 2.0 * PI) phase -= 2.0 * PI;
    modPhase += 2.0 * PI * colorVibrato / sampleRate;
    if (modPhase > 2.0 * PI) modPhase -= 2.0 * PI;
}
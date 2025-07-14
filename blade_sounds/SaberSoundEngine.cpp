#include "SaberSoundEngine.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

constexpr double PI = 3.14159265358979323846;
constexpr double VOLUME = 10.0;        // Output volume for hum
constexpr double BASE_FREQ = 65.0;     // Low end of hum (Hz)
constexpr double FREQ_RANGE = 190.0;   // How much color raises pitch (Hz)
constexpr double BASE_VIBRATO = 0.18;  // Vibrato depth (Hz)

SaberSoundEngine::SaberSoundEngine(double sr)
    : sampleRate(sr)
    , phase(0.0)
    , modPhase(0.0)
    , baseFreq(BASE_FREQ)
    , vibratoDepth(BASE_VIBRATO)
    , color(0, 255, 0)
    , state(SABER_HUM)
    , ignitionPos(0)
{}

void SaberSoundEngine::setColor(const SimpleRGB& c) {
    color = c;
}

void SaberSoundEngine::startIgnition() {
    generateIgnitionPCM(color);
    ignitionPos = 0;
    state = SABER_IGNITION;
}

void SaberSoundEngine::generateIgnitionPCM(const SimpleRGB& c) {
    ignitionPCM.clear();

    double r = c.r / 255.0, g = c.g / 255.0, b = c.b / 255.0;
    double total = r + g + b; if (total < 0.01) total = 1.0;
    r /= total; g /= total; b /= total;

    double baseFreq = 370.0 + r*160.0 + g*140.0 + b*150.0;
    double overtoneFreq = baseFreq * (1.4 + 0.27 * g);
    double duration = 0.34;
    int samples = int(sampleRate * duration);

    for (int i = 0; i < samples; ++i) {
        double t = double(i) / sampleRate;

        // ENVELOPE
        double attack = (t < 0.012) ? (1.0 - t/0.012) : 0.0;        // Snap
        double sweepEnv = (t < 0.06) ? (t/0.06) : 1.0;               // Sweep ramp

        // SNAP/NOISE (hi-hat): short, but less dominant
        double white = (rand()/(double)RAND_MAX)*2-1;
        double snap = 0.09*white*attack;

        // SWEEP (plasma): noisy + detuned, comes in under the snap, takes over
        double detune = std::sin(2*PI*(baseFreq+27.0)*t);
        double core = std::sin(2.0*PI*baseFreq*t + 0.20*detune) * sweepEnv;
        double overtone = std::sin(2.0*PI*overtoneFreq*t + 0.34*detune) * (0.15 + 0.16*g) * sweepEnv;

        // PLASMA NOISE (modulated)
        double plasma = 0.04*white*(0.7 + 0.3*sweepEnv);

        // MIX: Snap blends right into sweep/plasma, never drops out
        double sample = snap + core + overtone + plasma;

        // Soft clip
        sample = std::tanh(sample * 1.18);

        ignitionPCM.push_back(static_cast<float>(sample));
        ignitionPCM.push_back(static_cast<float>(sample));
    }
}

void SaberSoundEngine::nextSample(float& left, float& right) {
    if (state == SABER_IGNITION && ignitionPos < ignitionPCM.size()) {
        left  = ignitionPCM[ignitionPos++];
        right = ignitionPCM[ignitionPos++];
        if (ignitionPos >= ignitionPCM.size()) {
            state = SABER_HUM; // Done!
        }
        return;
    }

    // --- Hum section ---
    double r = color.r / 255.0;
    double g = color.g / 255.0;
    double b = color.b / 255.0;
    double total = r + g + b;
    if (total < 0.01) total = 1.0;
    r /= total; g /= total; b /= total;

    double colorFreq = BASE_FREQ + r*40.0 + g*65.0 + b*FREQ_RANGE;
    double colorVibrato = 1.5 + r*2.0 + g*3.5 + b*6.2;

    double vibrato = vibratoDepth * std::sin(modPhase);
    double freq = colorFreq + vibrato;

    double sample = std::sin(phase) * VOLUME;
    sample += 0.17 * std::sin(phase * 2.0) * VOLUME;

    left = right = (float)sample;

    phase    += 2.0 * PI * freq / sampleRate;
    if (phase > 2.0 * PI) phase -= 2.0 * PI;
    modPhase += 2.0 * PI * colorVibrato / sampleRate;
    if (modPhase > 2.0 * PI) modPhase -= 2.0 * PI;
}
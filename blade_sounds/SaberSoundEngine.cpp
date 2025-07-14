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

    // Convert RGB to HSV for intuitive color mapping
    double r = c.r / 255.0, g = c.g / 255.0, b = c.b / 255.0;
    double max_val = std::max({r, g, b});
    double min_val = std::min({r, g, b});
    double delta = max_val - min_val;
    double hue = 0.0, saturation = 0.0, value = max_val;
    if (delta > 0.001) {
        saturation = delta / max_val;
        if (max_val == r) hue = 60.0 * (g - b) / delta;
        else if (max_val == g) hue = 60.0 * (2.0 + (b - r) / delta);
        else hue = 60.0 * (4.0 + (r - g) / delta);
        if (hue < 0.0) hue += 360.0;
    }

    // Color-driven sound parameters
    double duration = 0.5 + 0.3 * (hue / 360.0); // Red: 0.5s (sharp), Blue: 0.8s (smooth)
    int samples = static_cast<int>(sampleRate * duration);
    double ramp_exponent = 1.2 + 0.6 * (1.0 - hue / 360.0); // Red: 1.8 (fast), Blue: 1.2 (slow)
    double base_freq_start = 100.0 + 50.0 * (hue / 360.0); // Red: 100 Hz, Blue: 150 Hz
    double base_freq_end = 370.0 + r * 160.0 + g * 140.0 + b * 150.0; // Match hum
    double overtone_freq = base_freq_end * (1.1 + 0.25 * saturation); // Vivid = richer
    double distortion_gain = 1.0 + 2.5 * (1.0 - value); // Darker = more distortion
    double snap_amt = 0.06 + 0.08 * (1.0 - value); // Darker = stronger snap
    double noise_mix = 0.2 + 0.35 * saturation; // Vivid = noisier
    double crackle_amt = 0.08 + 0.3 * saturation; // Vivid = more crackle
    double pulse_freq = 4.0 + 6.0 * (hue / 360.0); // Red: 4 Hz, Blue: 10 Hz
    double crackle_freq = 10.0 + 20.0 * (1.0 - hue / 360.0); // Red: 30 Hz, Blue: 10 Hz

    // Color-specific waveform and envelope shapes
    double snap_duration = 0.03 + 0.02 * (hue / 360.0); // Red: 30ms, Blue: 50ms
    double ramp_tau = 0.08 + 0.07 * (hue / 360.0); // Red: 80ms, Blue: 150ms
    double waveform_mix = 1.0 - hue / 360.0; // Red: sawtooth, Blue: sine
    double mod_depth = 0.2 + 0.3 * saturation; // Vivid = deeper modulation

    for (int i = 0; i < samples; ++i) {
        double t = static_cast<double>(i) / sampleRate;

        // Envelopes: Color-driven shapes
        double snap_env = (t < snap_duration) ? std::exp(-t / (snap_duration / 3.0)) : 0.0;
        double ramp_env;
        if (hue < 120.0) { // Red/Yellow: Sharp exponential rise
            ramp_env = 1.0 - std::exp(-t / ramp_tau);
        } else if (hue < 240.0) { // Green/Cyan: Linear rise
            ramp_env = std::min(t / ramp_tau, 1.0);
        } else { // Blue: S-shaped (sigmoid) rise
            ramp_env = 1.0 / (1.0 + std::exp(-10.0 * (t / duration - 0.5)));
        }
        double global_env = std::sin(M_PI * t / duration); // Bell-shaped curve

        // Snap: Color-driven character
        double white = (rand() / static_cast<double>(RAND_MAX)) * 2.0 - 1.0;
        double bandpass_freq = 800.0 + 800.0 * (hue / 360.0); // Red: 800 Hz, Blue: 1600 Hz
        double snap_noise_mix = 0.4 + 0.4 * (1.0 - hue / 360.0); // Red: noisy, Blue: tonal
        double bandpass = std::sin(2.0 * M_PI * bandpass_freq * t);
        double snap = snap_amt * (snap_noise_mix * white + (1.0 - snap_noise_mix) * bandpass) * snap_env;

        // Pitch ramp: Non-linear rise with color-driven waveform
        double ramp_progress = t / duration;
        double freq = base_freq_start + (base_freq_end - base_freq_start) * std::pow(ramp_progress, ramp_exponent);
        double phase = 2.0 * M_PI * freq * t;
        double pulse = mod_depth * std::sin(2.0 * M_PI * pulse_freq * t) * (0.5 + 0.5 * saturation);
        double detune = 0.15 * std::sin(2.0 * M_PI * (8.0 + 8.0 * (hue / 360.0)) * t); // Red: 8 Hz, Blue: 16 Hz
        // Waveform: Blend sine, sawtooth, and square
        double core;
        if (hue < 120.0) { // Red/Yellow: Sawtooth (gritty)
            core = std::sin(phase + detune + pulse) + waveform_mix * std::sin(2.0 * (phase + detune));
        } else if (hue < 240.0) { // Green/Cyan: Square-like (punchy)
            core = (std::sin(phase + detune + pulse) > 0.0) ? 1.0 : -1.0;
            core += 0.3 * std::sin(2.0 * (phase + detune)) * waveform_mix; // Fixed harmonic
        } else { // Blue: Sine (smooth)
            core = std::sin(phase + detune + pulse);
        }
        // Blend with modulated noise
        core = (1.0 - noise_mix) * core + noise_mix * white * (0.5 + 0.5 * std::sin(2.0 * M_PI * crackle_freq * t));
        core *= ramp_env;

        // Overtone: Noisy, color-driven
        double overtone = std::sin(2.0 * M_PI * overtone_freq * t + 0.2 * detune) * (0.04 + 0.12 * saturation);
        overtone += 0.08 * white * (0.5 + 0.5 * std::sin(2.0 * M_PI * crackle_freq * t)) * saturation;
        overtone *= ramp_env;

        // Plasma crackle: Color-driven
        double plasma = crackle_amt * white * (0.5 + 0.5 * std::sin(2.0 * M_PI * crackle_freq * t)) * ramp_env;

        // Mix components
        double sample = snap + core + overtone + plasma;

        // Distortion: Color-driven, dynamic
        sample *= (0.5 + 0.5 * ramp_progress); // Dynamic gain
        if (hue < 120.0) { // Red/Yellow (Sith)
            sample = std::tanh(distortion_gain * sample);
        } else if (hue < 240.0) { // Green/Cyan
            sample = std::tanh((1.0 + 0.5 * saturation) * sample);
        } else { // Blue
            sample = std::tanh((1.0 + 0.3 * saturation) * sample);
        }

        // Apply global envelope and scale
        sample *= global_env * 0.75; // Avoid clipping

        // Stereo output
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
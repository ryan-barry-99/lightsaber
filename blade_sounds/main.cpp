#include "SaberSoundEngine.h"
#include <portaudio.h>
#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <atomic>
#include <string>

SimpleRGB leds[1] = { SimpleRGB(255, 0, 0) }; // Start red
std::atomic<bool> running(true);

static int paCallback(const void *, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo*,
                      PaStreamCallbackFlags,
                      void *userData)
{
    SaberSoundEngine* saber = static_cast<SaberSoundEngine*>(userData);
    float *out = static_cast<float*>(outputBuffer);

    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float left, right;
        saber->nextSample(left, right);
        out[i*2 + 0] = left;
        out[i*2 + 1] = right;
    }
    return running ? paContinue : paComplete;
}

void do_swing(SaberSoundEngine& saber, std::mt19937& rng) {
    std::uniform_real_distribution<double> swingAccelDist(1.2, 1.7);
    const int swingSteps = 50;
    const int swingDurationMs = 340;
    const double swingIntervalMs = swingDurationMs / double(swingSteps);
    double peak = swingAccelDist(rng);

    for (int i = 0; i < swingSteps; ++i) {
        double t = double(i) / (swingSteps - 1); // 0 to 1
        double accel = peak * std::sin(3.14159 * t);
        saber.setAccel(accel);
        std::this_thread::sleep_for(std::chrono::milliseconds(int(swingIntervalMs)));
    }
    // Set to idle at the end
    saber.setAccel(0.02);
}

int main()
{
    constexpr double sampleRate = 44100.0;
    SaberSoundEngine saber(sampleRate, leds);

    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) { std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl; return 1; }

    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, (int)sampleRate, 256, paCallback, &saber);
    if (err != paNoError) { std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl; return 1; }

    err = Pa_StartStream(stream);
    if (err != paNoError) { std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl; return 1; }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> colorDist(0, 255);

    saber.setAccel(0.02);

    std::cout << "Cinematic saber: Press Enter for new color, Space + Enter for swing, q to quit.\n";
    std::string input;
    while (running) {
        std::getline(std::cin, input);
        if (input == "q" || input == "Q") {
            running = false;
            break;
        } else if (input.empty()) {
            leds[0] = SimpleRGB(colorDist(rng), colorDist(rng), colorDist(rng));
            std::cout << "New saber color (R,G,B): " << leds[0].r << ", " << leds[0].g << ", " << leds[0].b << std::endl;
        } else if (input == " ") {
            std::cout << "FULL SWING!" << std::endl;
            do_swing(saber, rng);
        } else {
            std::cout << "Press Enter for color, Space (and Enter) for swing, q to quit.\n";
        }
    }

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}
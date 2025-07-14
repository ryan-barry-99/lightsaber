#include "SaberSoundEngine.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <portaudio.h>

static int paCallback(const void* input, void* output,
                      unsigned long frameCount,
                      const PaStreamCallbackTimeInfo*,
                      PaStreamCallbackFlags,
                      void* userData)
{
    SaberSoundEngine* engine = static_cast<SaberSoundEngine*>(userData);
    float* out = static_cast<float*>(output);

    for (unsigned long i = 0; i < frameCount; ++i) {
        float left, right;
        engine->nextSample(left, right);
        out[2*i] = left;
        out[2*i+1] = right;
    }
    return paContinue;
}

int main() {
    double sampleRate = 44100;
    SaberSoundEngine engine(sampleRate);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 255);

    Pa_Initialize();
    PaStream* stream;
    Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, sampleRate, 256, paCallback, &engine);
    Pa_StartStream(stream);

    std::cout << "Press ENTER for new random color ignition + hum. Ctrl+C to exit.\n";

    while (true) {
        std::cin.get();
        int r = dist(gen), g = dist(gen), b = dist(gen);
        engine.setColor(SimpleRGB(r, g, b));
        engine.startIgnition();
        std::cout << "Ignition! (R=" << r << " G=" << g << " B=" << b << ")\n";
    }

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;
}
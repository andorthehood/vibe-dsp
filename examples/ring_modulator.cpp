// Ring Modulator - Metallic/robotic effect
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

// Parameters
float carrier_freq = 200.0f;  // Carrier frequency in Hz (50 - 1000)
float mix = 0.7f;             // Dry/wet mix (0.0 = dry, 1.0 = fully wet)

// Carrier oscillator
Oscillator carrier;

static void Callback(AudioHandle::InputBuffer in,
                     AudioHandle::OutputBuffer out,
                     size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        float inL = in[0][i];
        float inR = in[1][i];

        // Get carrier signal (sine wave)
        float mod = carrier.Process();

        // Ring modulate: multiply input by carrier
        float wetL = inL * mod;
        float wetR = inR * mod;

        // Output mix
        out[0][i] = inL * (1.0f - mix) + wetL * mix;
        out[1][i] = inR * (1.0f - mix) + wetR * mix;
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4);
    float sample_rate = hw.AudioSampleRate();

    // Initialize carrier oscillator
    carrier.Init(sample_rate);
    carrier.SetWaveform(Oscillator::WAVE_SIN);
    carrier.SetFreq(carrier_freq);
    carrier.SetAmp(1.0f);

    hw.StartAudio(Callback);
    while (1) {}
}

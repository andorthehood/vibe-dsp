// Octave Down - Simple octave divider
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

// Parameters
float mix = 0.7f;          // Dry/wet mix (0.0 = dry, 1.0 = fully wet)

// Octave tracking
float lastSampleL = 0.0f;
float lastSampleR = 0.0f;
bool flipL = false;
bool flipR = false;
float octaveL = 0.0f;
float octaveR = 0.0f;

// Low-pass filters to smooth the octave signal
OnePole lpfL, lpfR;

static void Callback(AudioHandle::InputBuffer in,
                     AudioHandle::OutputBuffer out,
                     size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        float inL = in[0][i];
        float inR = in[1][i];

        // Simple octave down: flip polarity on zero crossings
        // This creates a square wave at half frequency
        if ((lastSampleL <= 0.0f && inL > 0.0f) ||
            (lastSampleL >= 0.0f && inL < 0.0f))
        {
            flipL = !flipL;
        }
        if ((lastSampleR <= 0.0f && inR > 0.0f) ||
            (lastSampleR >= 0.0f && inR < 0.0f))
        {
            flipR = !flipR;
        }

        lastSampleL = inL;
        lastSampleR = inR;

        // Generate octave down by multiplying input with square wave
        octaveL = inL * (flipL ? 1.0f : -1.0f);
        octaveR = inR * (flipR ? 1.0f : -1.0f);

        // Filter to smooth out harshness
        octaveL = lpfL.Process(octaveL);
        octaveR = lpfR.Process(octaveR);

        // Output mix
        out[0][i] = inL * (1.0f - mix) + octaveL * mix;
        out[1][i] = inR * (1.0f - mix) + octaveR * mix;
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4);
    float sample_rate = hw.AudioSampleRate();

    // Initialize low-pass filters (cutoff around 400Hz to keep the sub smooth)
    lpfL.Init();
    lpfR.Init();
    lpfL.SetFrequency(400.0f / sample_rate);
    lpfR.SetFrequency(400.0f / sample_rate);

    hw.StartAudio(Callback);
    while (1) {}
}

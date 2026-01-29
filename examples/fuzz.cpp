// Fuzz - Classic fuzz/distortion effect
// Asymmetric soft clipping with tone control

#include "daisy_seed.h"
#include "daisysp.h"
#include <cmath>

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

// Fuzz parameters
float drive = 35.0f;      // Input gain (1.0 - 50.0, higher = more fuzz)
float tone = 0.4f;        // Tone control (0.0 = dark, 1.0 = bright)
float level = 0.3f;       // Output level (0.0 - 1.0)

// Tone filter (low-pass)
OnePole toneFilterL, toneFilterR;

// Soft clipping function for fuzz character
inline float FuzzClip(float x)
{
    // Apply drive
    x *= drive;

    // Asymmetric soft clipping for classic fuzz character
    if (x > 0.0f)
        x = 1.0f - expf(-x);
    else
        x = -1.0f + expf(x);

    return x;
}

static void Callback(AudioHandle::InputBuffer in,
                     AudioHandle::OutputBuffer out,
                     size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        // Process left channel
        float sigL = FuzzClip(in[0][i]);
        sigL = toneFilterL.Process(sigL);
        out[0][i] = sigL * level;

        // Process right channel
        float sigR = FuzzClip(in[1][i]);
        sigR = toneFilterR.Process(sigR);
        out[1][i] = sigR * level;
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4);
    float sample_rate = hw.AudioSampleRate();

    // Initialize tone filters
    toneFilterL.Init();
    toneFilterR.Init();

    // Map tone parameter to frequency range (200Hz - 6000Hz)
    // OnePole uses normalized frequency (0 to 0.497)
    float tone_freq_hz = 200.0f + tone * 5800.0f;
    float normalized_freq = tone_freq_hz / sample_rate;
    toneFilterL.SetFrequency(normalized_freq);
    toneFilterR.SetFrequency(normalized_freq);

    hw.StartAudio(Callback);
    while (1) {}
}

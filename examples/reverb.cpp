// Reverb - Feedback delay network reverb
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

// Reverb parameters
float decay = 0.85f;      // Reverb decay (0.0 - 0.95, higher = longer tail)
float mix = 0.4f;         // Dry/wet mix (0.0 = dry, 1.0 = fully wet)
float damping = 0.3f;     // High frequency damping (0.0 - 0.9)

// Delay lines for reverb (prime-ish lengths for less metallic sound)
static DelayLine<float, 3617> delayL1;
static DelayLine<float, 3163> delayR1;
static DelayLine<float, 2473> delayL2;
static DelayLine<float, 2767> delayR2;

// Allpass diffusers
static DelayLine<float, 241> apL1;
static DelayLine<float, 197> apR1;
static DelayLine<float, 113> apL2;
static DelayLine<float, 137> apR2;

// Low-pass filters for damping
OnePole lpfL, lpfR;

// Feedback storage
float feedbackL = 0.0f;
float feedbackR = 0.0f;

static void Callback(AudioHandle::InputBuffer in,
                     AudioHandle::OutputBuffer out,
                     size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        float inL = in[0][i];
        float inR = in[1][i];

        // Read from delay lines
        float dL1 = delayL1.Read();
        float dR1 = delayR1.Read();
        float dL2 = delayL2.Read();
        float dR2 = delayR2.Read();

        // Mix delay taps
        float wetL = (dL1 + dL2) * 0.5f;
        float wetR = (dR1 + dR2) * 0.5f;

        // Apply damping (low-pass filter)
        wetL = lpfL.Process(wetL);
        wetR = lpfR.Process(wetR);

        // Cross-feedback for stereo spread
        float fbL = (wetL + wetR * 0.3f) * decay;
        float fbR = (wetR + wetL * 0.3f) * decay;

        // Allpass diffusion
        float apOutL = apL1.Allpass(fbL, 241, 0.6f);
        apOutL = apL2.Allpass(apOutL, 113, 0.6f);
        float apOutR = apR1.Allpass(fbR, 197, 0.6f);
        apOutR = apR2.Allpass(apOutR, 137, 0.6f);

        // Write to delay lines (input + feedback)
        delayL1.Write(inL + apOutL);
        delayR1.Write(inR + apOutR);
        delayL2.Write(inL + apOutR * 0.7f);  // Cross-feed for width
        delayR2.Write(inR + apOutL * 0.7f);

        // Output mix
        out[0][i] = inL * (1.0f - mix) + wetL * mix;
        out[1][i] = inR * (1.0f - mix) + wetR * mix;
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4);

    // Initialize delay lines
    delayL1.Init();
    delayR1.Init();
    delayL2.Init();
    delayR2.Init();
    delayL1.SetDelay(static_cast<size_t>(3617));
    delayR1.SetDelay(static_cast<size_t>(3163));
    delayL2.SetDelay(static_cast<size_t>(2473));
    delayR2.SetDelay(static_cast<size_t>(2767));

    // Initialize allpass diffusers
    apL1.Init();
    apR1.Init();
    apL2.Init();
    apR2.Init();

    // Initialize damping filters
    lpfL.Init();
    lpfR.Init();
    float dampFreq = (1.0f - damping) * 0.4f + 0.05f;
    lpfL.SetFrequency(dampFreq);
    lpfR.SetFrequency(dampFreq);

    hw.StartAudio(Callback);
    while (1) {}
}

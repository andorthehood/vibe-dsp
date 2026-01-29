// Bypass - Simple passthrough (template for new effects)
// Audio passes through unchanged

#include "daisy_seed.h"

using namespace daisy;

DaisySeed hw;

static void Callback(AudioHandle::InputBuffer in,
                     AudioHandle::OutputBuffer out,
                     size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        out[0][i] = in[0][i];
        out[1][i] = in[1][i];
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4);
    hw.StartAudio(Callback);
    while (1) {}
}

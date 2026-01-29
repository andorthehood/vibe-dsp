# vibe-dsp Agent Instructions

This file provides context for AI assistants working on this project.

## Project Overview

This is a DSP effects project for the **Daisy Seed** microcontroller. The goal is rapid prototyping of audio effects through simple natural language requests.

## Hardware Setup

- **Platform**: Daisy Seed
- **Audio**: Stereo input and output (use both channels)
- **Controls**: None - no knobs, buttons, or CV inputs
- **All parameters must be hardcoded** in the source code

## Workflow

1. User requests an effect (e.g., "make a phaser effect")
2. Edit `src/main.cpp` with the DSP implementation
3. Build and flash: `make build_and_program` (uses ST-Link)

**When the user asks for a different effect, start fresh.** Replace the entire contents of `main.cpp` rather than modifying the existing effect.

## Code Structure

- **Main source**: `src/main.cpp`
- **Examples**: `examples/` - Reference implementations to learn from
- **Saved**: `saved/` - User's saved effects to roll back to
- **Libraries**:
  - `libs/libDaisy` - Hardware abstraction
  - `libs/DaisySP` - DSP building blocks (filters, delays, oscillators, etc.)

**Important**: Before writing a new effect, check `examples/` for similar implementations.

To load a saved effect: copy it to `src/main.cpp` and run `make build_and_program`.

## Coding Guidelines

### Template Structure
```cpp
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

// Effect parameters (hardcoded)
// e.g., float drive = 0.7f;

// DSP objects
// e.g., Overdrive overdrive;

static void Callback(AudioHandle::InputBuffer in,
                     AudioHandle::OutputBuffer out,
                     size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        // Process left and right channels
        out[0][i] = /* processed */ in[0][i];
        out[1][i] = /* processed */ in[1][i];
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4);
    float sample_rate = hw.AudioSampleRate();

    // Initialize DSP objects here
    // e.g., overdrive.Init();

    hw.StartAudio(Callback);
    while (1) {}
}
```

### Parameter Adjustments

When user asks to adjust a parameter (e.g., "increase the fuzz" or "make delay longer"):
- Locate the hardcoded parameter in main.cpp
- Adjust the value appropriately
- Rebuild and flash

**If the user doesn't specify exact values, just pick reasonable defaults.** Don't ask - make a choice and flash it. The user will tell you if they want it adjusted.

### Audio Processing

- Sample rate: Use `hw.AudioSampleRate()` (typically 48kHz)
- Block size: 4 samples (low latency)
- Always process both stereo channels
- Keep processing efficient - this runs in real-time

## Available DaisySP Modules

Common effects to use:
- `Overdrive` - Soft clipping distortion
- `Phaser` - Phase shifting effect
- `Chorus` - Chorus/ensemble effect
- `Flanger` - Flanging effect
- `ReverbSc` - Stereo reverb
- `DelayLine` - Delay/echo effects
- `Tremolo` - Amplitude modulation
- `OnePole` / `Svf` - Filters (OnePole uses normalized freq: hz/sample_rate)
- `Oscillator` - LFOs and signal generation
- `Compressor` - Dynamics processing

## Build Commands

- `make build_and_program` - Clean, build, and flash via ST-Link (preferred)
- `make build_and_program_dfu` - Clean, build, and flash via USB DFU
- `make build_libs` - Rebuild libDaisy and DaisySP libraries

## Example Requests

These kinds of simple prompts should work:
- "make a fuzz effect"
- "add reverb"
- "create a phaser"
- "increase the drive"
- "make the delay time longer"
- "add more feedback to the delay"
- "make it sound more subtle"

## Examples Library

Reference implementations in `examples/`:

| File | Description |
|------|-------------|
| `bypass.cpp` | Minimal template - audio passthrough |
| `fuzz.cpp` | Fuzz/distortion with drive, tone, and level controls |
| `reverb.cpp` | FDN reverb with decay, mix, and damping controls |
| `ring_modulator.cpp` | Ring modulator with carrier frequency control |

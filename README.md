# vibe-dsp

Audio effects for Daisy Seed, built with natural language.

Tell an AI assistant what effect you want ("make a fuzz", "add reverb", "increase the drive") and it writes the DSP code, builds, and flashes to hardware.

## Requirements

- Daisy Seed
- ST-Link programmer
- ARM GCC toolchain

## Setup

```bash
git submodule update --init --recursive
make build_libs
```

## Usage

```bash
make build_and_program
```

See `AGENTS.md` for AI assistant instructions.

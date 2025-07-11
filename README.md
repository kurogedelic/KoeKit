# KoeKit - High-Performance Audio Synthesis for RP2350A

KoeKit is a high-performance audio synthesis library designed specifically for RP2350A microcontrollers. It provides real-time audio generation with modular oscillators, filters, and effects, optimized for embedded applications.

## Features

- **RP2350A Optimized**: Takes advantage of FPU, PIO, and dual-core architecture
- **Modular Design**: Mix and match oscillators, filters, and effects
- **Complete Audio Toolkit**: 
  - High-quality wavetable oscillators with 6+ built-in waveforms
  - Professional filters (OnePole, StateVariable, Biquad)
  - ADSR/AR envelopes and LFO modulators
  - Custom wavetable creation system
- **Compile-Time Optimization**: Wavetables and processing chains optimized at compile time
- **Low Latency**: < 3ms audio processing latency
- **Memory Efficient**: < 32KB RAM footprint for full-featured synthesis
- **Extensible**: Easy to add custom waveforms and effects

## Quick Start

### Installation

1. Download or clone this repository
2. Copy the `KoeKit` folder to your Arduino libraries directory
3. Install the `arduino-pico` board package (version 4.0.0 or later)

### Basic Usage

```cpp
#include <KoeKit.h>

// Create a sine wave oscillator
KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);

void setup() {
  // Initialize KoeKit (22050 Hz, pin 1)
  KoeKit::begin(22050, 1);
  
  // Configure oscillator
  osc.setFrequency(440.0f);  // A4 note
  osc.setAmplitude(0.3f);    // 30% volume
  
  // Set audio callback
  KoeKit::setAudioCallback([]() -> float {
    return osc.process();
  });
}

void loop() {
  // Audio runs in background via interrupts
}
```

## Hardware Requirements

- **RP2350A-based board** (Raspberry Pi Pico 2 or compatible)
- **Speaker or headphones** connected to PWM output pin
- **Optional**: Low-pass filter for improved audio quality

### Basic Circuit

```
RP2350A Pin 1 -> [1kΩ resistor] -> [Speaker] -> GND
              -> [100nF capacitor] -> GND (optional filter)
```

## Core Components

### Wavetable System

KoeKit uses compile-time generated wavetables for high-quality oscillators:

```cpp
// Use built-in waveforms
auto osc1 = KoeKit::createOscillator(KoeKit::Wavetables::Basic::Waveform::SINE);
auto osc2 = KoeKit::createOscillator(KoeKit::Wavetables::Basic::Waveform::SAW);

// Create custom waveforms
constexpr auto myWave = KoeKit::makeWavetable<1024>([](size_t i) -> float {
  float phase = 2.0f * M_PI * static_cast<float>(i) / 1024.0f;
  return std::sin(phase) + 0.3f * std::sin(3.0f * phase);
});

KoeKit::Oscillator customOsc(myWave);
```

### Available Waveforms

- **Sine**: Pure sine wave
- **Sawtooth**: Bright, buzzy sawtooth
- **Square**: Classic square wave
- **Triangle**: Soft triangle wave
- **Soft Sawtooth**: Band-limited sawtooth with reduced harmonics
- **Pulse**: 25% duty cycle pulse wave

### Performance Specifications

- **Sample Rate**: Up to 48kHz (22kHz recommended)
- **Latency**: < 3ms at 22kHz
- **CPU Usage**: < 20% at 150MHz (typical synthesis)
- **Memory**: 16KB base + wavetables
- **Polyphony**: 4-8 voices depending on complexity

## Examples

The library includes several example sketches:

**Basic Examples:**
- **SimpleTone**: Basic 440Hz sine wave
- **WaveformTest**: Cycle through all basic waveforms
- **FrequencySweep**: Smooth frequency transitions
- **CustomWavetable**: Create and use custom waveforms

**Advanced Examples:**
- **FilterSweep**: Resonant filter frequency sweeping
- **EnvelopeSynth**: Complete synthesizer with ADSR and filter envelopes
- **DrumMachine**: Percussion synthesizer with sequencer

## Advanced Features

### Custom Wavetable Creation

```cpp
// Mathematical formula
constexpr auto harmonicWave = KoeKit::makeWavetable<1024>([](size_t i) -> float {
  float phase = 2.0f * M_PI * static_cast<float>(i) / 1024.0f;
  return 0.5f * sin(phase) + 0.3f * sin(2.0f * phase) + 0.2f * sin(3.0f * phase);
});

// From sample array
std::array<float, 1024> samples = { /* your samples */ };
constexpr auto sampleWave = KoeKit::makeWavetable(samples);
```

### Complete Synthesizer with Filter and Envelope

```cpp
KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SAW);
KoeKit::Filter::StateVariable filter;
KoeKit::Envelope::ADSR envelope;

void setup() {
  KoeKit::begin(22050, 1);
  
  osc.setFrequency(220.0f);
  filter.setParams(1000.0f, 2.0f);  // 1kHz cutoff, resonance = 2
  envelope.setADSR(0.01f, 0.1f, 0.7f, 0.3f);
  
  KoeKit::setAudioCallback([]() -> float {
    float sample = osc.process();
    filter.process(sample);
    sample = filter.getLowPass();
    return envelope.process(sample);
  });
  
  envelope.noteOn();  // Start the envelope
}
```

## Configuration

### Compile-Time Options

```cpp
// In your sketch or build system
#define KOEKIT_SAMPLE_RATE 22050      // Default: 22050
#define KOEKIT_WAVETABLE_SIZE 1024    // Default: 1024
```

### Runtime Configuration

```cpp
// Different sample rates
KoeKit::begin(44100, 1);  // 44.1kHz
KoeKit::begin(22050, 1);  // 22kHz (recommended)

// Different output pins
KoeKit::begin(22050, 2);  // Use pin 2 for output
```

## Memory Usage

| Component | Memory Usage |
|-----------|--------------|
| Base system | 8KB |
| Basic wavetables (6 waves) | 12KB |
| Each additional 1024-sample table | 2KB |
| Filter instances | ~20 bytes each |
| Envelope instances | ~30 bytes each |
| Audio buffers | 2KB |

## Future Roadmap

- **✅ Filters**: Low-pass, high-pass, band-pass filters (COMPLETED)
- **✅ Envelopes**: ADSR and custom envelope generators (COMPLETED)
- **PIO Audio Output**: I2S-quality timing using RP2350A PIO
- **Effects**: Reverb, delay, distortion
- **I2S Output**: High-quality digital audio output
- **MIDI Support**: Hardware and USB MIDI input
- **Polyphonic Engine**: Voice management system

## Contributing

Contributions are welcome! Please see our contribution guidelines and feel free to submit issues and pull requests.

## License

MIT License - See LICENSE file for details.

## Documentation

- **[Getting Started Guide](docs/getting-started.md)**: Complete setup and first steps
- **[API Reference](docs/api.md)**: Complete API documentation
- **[Examples Guide](docs/examples.md)**: Detailed examples and tutorials

## Support

- **Issues**: [GitHub Issues](https://github.com/kurogedelic/KoeKit/issues)
- **Discussions**: [GitHub Discussions](https://github.com/kurogedelic/KoeKit/discussions)
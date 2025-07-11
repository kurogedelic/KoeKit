---
layout: home

hero:
  name: "KoeKit"
  text: "High-Performance Audio Synthesis"
  tagline: "Real-time audio generation for RP2350A microcontrollers"
  image:
    src: /logo.svg
    alt: KoeKit
  actions:
    - theme: brand
      text: Get Started
      link: /getting-started
    - theme: alt
      text: View on GitHub
      link: https://github.com/kurogedelic/KoeKit

features:
  - icon: ⚡
    title: RP2350A Optimized
    details: Takes full advantage of FPU, PIO, and dual-core architecture for maximum performance
  - icon: 🎵
    title: Complete Audio Toolkit
    details: High-quality oscillators, professional filters, ADSR envelopes, and LFO modulators
  - icon: 🔧
    title: Modular Design
    details: Mix and match oscillators, filters, and effects to create complex synthesis chains
  - icon: 📈
    title: High Performance
    details: <3ms latency, <20% CPU usage, and <32KB memory footprint for full-featured synthesis
  - icon: 🎯
    title: Compile-Time Optimization
    details: Wavetables and processing chains optimized at compile time for zero runtime overhead
  - icon: 🔌
    title: Easy to Use
    details: Simple Arduino-style API with comprehensive examples and documentation
---

## Quick Example

Get started with KoeKit in just a few lines of code:

```cpp
#include <KoeKit.h>

// Create a sine wave oscillator
KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);

void setup() {
  // Initialize KoeKit
  KoeKit::begin(22050, 1);
  
  // Configure oscillator
  osc.setFrequency(440.0f);  // A4 note
  osc.setAmplitude(0.3f);    // Safe volume
  
  // Set audio callback
  KoeKit::setAudioCallback([]() -> float {
    return osc.process();
  });
}

void loop() {
  // Audio runs automatically in background
}
```

## Advanced Synthesis

Create complex sounds with filters and envelopes:

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SAW);
KoeKit::Filter::StateVariable filter;
KoeKit::Envelope::ADSR envelope;

void setup() {
  KoeKit::begin(22050, 1);
  
  // Configure synthesis chain
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

## Hardware Requirements

- **RP2350A-based board** (Raspberry Pi Pico 2 or compatible)
- **Speaker or headphones** connected to PWM output pin
- **Optional**: Low-pass filter for improved audio quality

## Key Features

### 🎛️ Audio Sources
- **Wavetable Oscillators**: 6 built-in waveforms + custom wavetable support
- **Noise Generator**: Fast XorShift-based pseudo-random noise
- **Phase Accuracy**: Double-precision phase tracking for minimal drift

### 🔊 Audio Processing  
- **Filters**: OnePole, StateVariable, Biquad, DCBlocker
- **Envelopes**: ADSR, AR (Attack-Release), Multi-waveform LFO
- **Real-time Control**: All parameters adjustable during playback

### ⚙️ Advanced Features
- **Custom Wavetables**: Create wavetables from mathematical formulas or sample arrays
- **Compile-time Generation**: Zero runtime overhead for wavetable creation
- **Memory Efficient**: Selective inclusion of only needed components

## Performance Specifications

| Metric | Value |
|--------|-------|
| **Sample Rate** | Up to 48kHz (22kHz recommended) |
| **Latency** | < 3ms at 22kHz |
| **CPU Usage** | < 20% at 150MHz (typical synthesis) |
| **Memory** | 22KB base + wavetables |
| **Polyphony** | 4-8 voices depending on complexity |

## Get Started

Ready to create amazing sounds with your RP2350A? Choose your path:

- **[Installation Guide](/getting-started)** - Set up KoeKit in your Arduino IDE
- **[Quick Start](/quick-start)** - Get your first sound playing in minutes  
- **[Hardware Setup](/hardware-setup)** - Connect speakers and build audio circuits
- **[API Reference](/api/)** - Complete function documentation
- **[Examples](/examples/)** - From simple tones to complete synthesizers
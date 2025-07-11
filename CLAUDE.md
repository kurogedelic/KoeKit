# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

# KoeKit Audio Synthesis Library - Development Guide

## Project Overview
KoeKit is a high-performance Arduino audio synthesis library targeting RP2040/2350 microcontrollers with future ESP32 support. Designed for real-time audio synthesis with minimal CPU overhead and optimized for embedded applications.

## Current Status
⚠️ **Project is in planning/design phase** - Only documentation exists currently. The planned file structure and implementation described below needs to be created.

## Development Commands

### Project Setup
Since this is an Arduino library, there are no traditional build commands. Development workflow:

```bash
# For testing with Arduino IDE:
# 1. Copy library to Arduino libraries directory
cp -r . ~/Arduino/libraries/KoeKit/

# 2. Open Arduino IDE and create test sketch
# 3. Include library: #include <KoeKit.h>
# 4. Compile and upload to RP2040/RP2350 board
```

### Testing
- **Unit Tests**: Use Unity testing framework in `test/` directory
- **Hardware Tests**: Compile and run examples on actual RP2040/2350 hardware
- **Performance Tests**: Measure CPU usage and latency on target hardware

### Library Management
```bash
# Validate library structure
arduino-cli lib list | grep KoeKit

# Check library properties
cat library.properties
```

## Architecture Principles
- **Hardware-Specific**: Optimized for Arduino-pico (RP2040/2350)
- **Real-Time**: < 3ms latency audio processing
- **Modular Design**: Independent audio source and processor classes  
- **Future-Proof**: Extensible to ESP32 and other platforms
- **Memory Efficient**: < 16KB RAM footprint
- **Performance First**: < 30% CPU usage at 133MHz

## Core Architecture Concepts

### Class Hierarchy
The library follows a strict inheritance pattern:
- **AudioSource**: Base class for all audio generators (oscillators, noise generators)
- **AudioProcessor**: Base class for single-input effects (filters, envelopes)
- **AudioMultiProcessor**: Base class for multi-input processors (mixers, modulators)

### Data Flow Pattern
Audio flows through a chain: `AudioSource → AudioProcessor → AudioProcessor → PWMOutput`

Example: `WavetableOsc → Filter → ADSR → PWMOutput`

### Memory Management
- **Static allocation only** - no malloc/free in audio path
- **Compile-time configuration** - buffer sizes known at compile time
- **Lookup tables** - pre-calculated waveforms and math functions stored in PROGMEM

## Development Guidelines

### File Structure
```
KoeKit/
├── src/
│   ├── KoeKit.h              # Main header with all includes
│   ├── core/
│   │   ├── AudioEngine.h     # Core audio engine
│   │   ├── AudioEngine.cpp
│   │   ├── AudioSource.h     # Base classes
│   │   └── AudioProcessor.h
│   ├── sources/
│   │   ├── WavetableOsc.h    # Oscillators
│   │   ├── WavetableOsc.cpp
│   │   ├── BasicOsc.h
│   │   └── BasicOsc.cpp
│   ├── processors/
│   │   ├── Mixer.h           # Audio processors
│   │   ├── Mixer.cpp
│   │   ├── Filter.h
│   │   ├── Filter.cpp
│   │   ├── ADSR.h
│   │   └── ADSR.cpp
│   ├── io/
│   │   ├── PWMOutput.h       # Hardware I/O
│   │   ├── PWMOutput.cpp
│   │   ├── ADCInput.h
│   │   ├── ADCInput.cpp
│   │   ├── DigitalInput.h
│   │   └── DigitalInput.cpp
│   ├── utils/
│   │   ├── Math.h            # Math utilities
│   │   └── Math.cpp
│   └── platform/
│       ├── RP2040.h          # Platform-specific optimizations
│       ├── RP2040.cpp
│       ├── RP2350.h
│       └── RP2350.cpp
├── examples/
│   ├── BasicSynth/
│   ├── FilteredOsc/
│   ├── DrumMachine/
│   └── PolyphonicSynth/
├── library.properties
├── keywords.txt
├── API.md                    # API documentation
├── README.md
└── LICENSE
```

### Implementation Standards

#### Code Style
- **Naming**: PascalCase for classes, camelCase for methods/variables
- **Namespacing**: All classes in `KoeKit` namespace
- **Headers**: Include guards and forward declarations
- **Documentation**: Doxygen-style comments for all public APIs

#### Performance Requirements
- **Sample Rate**: Support up to 44.1kHz, optimize for 22kHz
- **Memory**: Static allocation preferred, avoid dynamic allocation
- **CPU**: Target < 30% usage at 133MHz for full audio chain
- **Latency**: Audio processing must complete within sample period

#### Example Implementation Pattern
```cpp
// WavetableOsc.h
#ifndef KOEKIT_WAVETABLE_OSC_H
#define KOEKIT_WAVETABLE_OSC_H

#include "core/AudioSource.h"

namespace KoeKit {
    /**
     * @brief High-quality wavetable oscillator
     * 
     * Supports multiple waveforms with anti-aliasing
     * and smooth frequency transitions.
     */
    class WavetableOsc : public AudioSource {
    public:
        WavetableOsc();
        virtual ~WavetableOsc() = default;
        
        void setWaveform(WaveformType waveform);
        void setFrequency(float frequency);
        
        float process() override;
        void reset() override;
        void setSampleRate(uint32_t sampleRate) override;
        
    private:
        static constexpr size_t WAVETABLE_SIZE = 1024;
        static float sineTable[WAVETABLE_SIZE];
        static float triangleTable[WAVETABLE_SIZE];
        // ... other tables
        
        WaveformType currentWaveform;
        float frequency;
        float phase;
        float phaseIncrement;
        uint32_t sampleRate;
        
        void updatePhaseIncrement();
        float interpolate(const float* table, float index);
    };
}

#endif // KOEKIT_WAVETABLE_OSC_H
```

### Platform Integration

#### RP2040/2350 Specific Features
- **Dual Core**: Utilize second core for audio processing
- **PIO**: Use PIO for high-precision timing
- **DMA**: Implement DMA-based audio output
- **Overclocking**: Support safe overclocking for higher performance

#### Hardware Abstraction
```cpp
// Platform-specific defines
#ifdef ARDUINO_ARCH_RP2040
    #define KOEKIT_PWM_SLICE pwm_gpio_to_slice_num
    #define KOEKIT_PWM_CHANNEL pwm_gpio_to_channel
#endif

#ifdef ARDUINO_ARCH_RP2350
    // RP2350-specific optimizations
    #define KOEKIT_USE_HSTX 1
#endif
```

### Testing Strategy

#### Unit Tests
- **Waveform Accuracy**: Verify oscillator output against reference
- **Filter Response**: Test frequency response curves
- **Performance**: Benchmark CPU usage and memory consumption
- **Platform**: Test on actual RP2040/2350 hardware

#### Example Test Structure
```cpp
// test/test_wavetable_osc.cpp
#include <unity.h>
#include "sources/WavetableOsc.h"

void test_sine_wave_accuracy() {
    KoeKit::WavetableOsc osc;
    osc.setSampleRate(44100);
    osc.setWaveform(KoeKit::SINE);
    osc.setFrequency(440.0);
    
    // Test for expected sine wave characteristics
    float samples[100];
    for (int i = 0; i < 100; i++) {
        samples[i] = osc.process();
    }
    
    // Verify RMS, frequency content, etc.
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.707, calculateRMS(samples, 100));
}
```

### Build System

#### library.properties
```ini
name=KoeKit
version=1.0.0
author=KoeKit Team
maintainer=development@koekit.audio
sentence=High-performance audio synthesis library for Arduino-pico
paragraph=Real-time audio synthesis optimized for RP2040/2350 microcontrollers
category=Audio
url=https://github.com/username/KoeKit
architectures=rp2040,rp2350
depends=arduino-pico (>=3.0.0)
```

#### Compilation Flags
```cpp
// Optimization flags for audio performance
#pragma GCC optimize("O3")
#pragma GCC target("arch=armv6-m")

// Platform-specific optimizations
#ifdef ARDUINO_ARCH_RP2040
    #define KOEKIT_FAST_MATH 1
    #define KOEKIT_USE_TABLES 1
#endif
```

### API Evolution

#### Version Strategy
- **Major**: Breaking API changes
- **Minor**: New features, backward compatible
- **Patch**: Bug fixes, performance improvements

#### Backward Compatibility
- Maintain API compatibility within major versions
- Deprecation warnings for removed features
- Migration guides for major version updates

### Documentation Standards

#### API Documentation
- Complete Doxygen documentation for all public APIs
- Usage examples for each class
- Performance characteristics and limitations
- Platform-specific notes

#### Example Documentation
```cpp
/**
 * @brief Sets the oscillator frequency
 * 
 * @param frequency Frequency in Hz (20.0 to 20000.0)
 * 
 * @note Frequency changes are smoothed to prevent clicks
 * @warning Frequencies above Nyquist frequency will be clamped
 * 
 * @performance O(1) - constant time operation
 * @memory No additional memory allocation
 * 
 * @example
 * ```cpp
 * KoeKit::WavetableOsc osc;
 * osc.setFrequency(440.0); // A4 note
 * ```
 */
void setFrequency(float frequency);
```

### Release Process

#### Quality Gates
1. **Unit Tests**: All tests must pass
2. **Performance Tests**: CPU usage within targets
3. **Hardware Tests**: Verified on RP2040/2350
4. **Documentation**: Complete API docs
5. **Examples**: Working example sketches

#### Release Checklist
- [ ] Version bump in library.properties
- [ ] Update API.md with new features
- [ ] Regenerate documentation
- [ ] Tag release in git
- [ ] Submit to Arduino Library Manager
- [ ] Update example projects

### Community Guidelines

#### Contribution Process
1. Fork repository
2. Create feature branch
3. Implement with tests
4. Update documentation
5. Submit pull request
6. Code review and integration

#### Issue Reporting
- Performance issues with benchmarks
- Hardware compatibility problems
- API improvement suggestions
- Bug reports with minimal reproduction cases

### Future Roadmap

#### Platform Expansion
- **ESP32**: Port core functionality
- **STM32**: Evaluate feasibility
- **Teensy**: Native audio library integration

#### Feature Additions
- **Effects**: Reverb, delay, distortion
- **Sequencing**: Built-in pattern sequencer
- **MIDI**: Hardware and USB MIDI support
- **Polyphony**: Voice management system

#### Performance Targets
- **RP2040**: 8-voice polyphony at 22kHz
- **RP2350**: 16-voice polyphony at 44kHz
- **Memory**: < 32KB for full feature set
- **Latency**: < 1ms on next-generation hardware
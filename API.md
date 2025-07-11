# KoeKit Audio Synthesis Library API

**Version**: 1.0.0  
**Platform**: Arduino-pico (RP2040/2350)  
**Future**: ESP32 support planned

## Overview

KoeKit is a high-performance audio synthesis library designed for Arduino-pico platforms, optimized for real-time audio generation on RP2040 and RP2350 microcontrollers.

## Core Architecture

### Audio Engine

```cpp
namespace KoeKit {
    class AudioEngine {
    public:
        static bool init(uint32_t sampleRate = 22050, uint16_t bufferSize = 64);
        static void setSampleRate(uint32_t sampleRate);
        static uint32_t getSampleRate();
        static void setBufferSize(uint16_t bufferSize);
        static uint16_t getBufferSize();
        static void shutdown();
    };
}
```

### Base Classes

```cpp
namespace KoeKit {
    class AudioSource {
    public:
        virtual float process() = 0;
        virtual void reset() = 0;
        virtual void setSampleRate(uint32_t sampleRate) = 0;
    };

    class AudioProcessor {
    public:
        virtual float process(float input) = 0;
        virtual void reset() = 0;
        virtual void setSampleRate(uint32_t sampleRate) = 0;
    };

    class AudioMultiProcessor {
    public:
        virtual float process(float input1, float input2) = 0;
        virtual void reset() = 0;
        virtual void setSampleRate(uint32_t sampleRate) = 0;
    };
}
```

## Audio Sources

### Wavetable Oscillator

```cpp
namespace KoeKit {
    enum WaveformType {
        SINE,
        TRIANGLE,
        SAWTOOTH,
        SQUARE,
        NOISE
    };

    class WavetableOsc : public AudioSource {
    public:
        WavetableOsc();
        
        void setWaveform(WaveformType waveform);
        void setFrequency(float frequency);
        void setAmplitude(float amplitude);
        void setPhase(float phase);
        
        float process() override;
        void reset() override;
        void setSampleRate(uint32_t sampleRate) override;
        
    private:
        WaveformType currentWaveform;
        float frequency;
        float amplitude;
        float phase;
        float phaseIncrement;
        uint32_t sampleRate;
    };
}
```

### Basic Oscillator

```cpp
namespace KoeKit {
    class BasicOsc : public AudioSource {
    public:
        BasicOsc();
        
        void setWaveform(WaveformType waveform);
        void setFrequency(float frequency);
        void setPulseWidth(float width); // For pulse waves
        void setAmplitude(float amplitude);
        
        float process() override;
        void reset() override;
        void setSampleRate(uint32_t sampleRate) override;
        
    private:
        WaveformType waveform;
        float frequency;
        float pulseWidth;
        float amplitude;
        float phase;
        uint32_t sampleRate;
    };
}
```

## Audio Processors

### Mixer

```cpp
namespace KoeKit {
    enum MixMode {
        ADD,
        MULTIPLY,
        CROSSFADE
    };

    class Mixer : public AudioMultiProcessor {
    public:
        Mixer();
        
        void setLevels(float level1, float level2);
        void setMixMode(MixMode mode);
        
        float process(float input1, float input2) override;
        void reset() override;
        void setSampleRate(uint32_t sampleRate) override;
        
    private:
        float level1, level2;
        MixMode mixMode;
    };
}
```

### Filter

```cpp
namespace KoeKit {
    enum FilterType {
        LOWPASS,
        HIGHPASS,
        BANDPASS,
        NOTCH
    };

    class Filter : public AudioProcessor {
    public:
        Filter();
        
        void setType(FilterType type);
        void setCutoff(float cutoff);
        void setResonance(float resonance);
        
        float process(float input) override;
        void reset() override;
        void setSampleRate(uint32_t sampleRate) override;
        
    private:
        FilterType filterType;
        float cutoff;
        float resonance;
        float x1, x2, y1, y2; // Biquad state
        uint32_t sampleRate;
    };
}
```

### ADSR Envelope

```cpp
namespace KoeKit {
    enum EnvelopeStage {
        IDLE,
        ATTACK,
        DECAY,
        SUSTAIN,
        RELEASE
    };

    class ADSR : public AudioProcessor {
    public:
        ADSR();
        
        void setADSR(float attack, float decay, float sustain, float release);
        void noteOn();
        void noteOff();
        bool isActive();
        
        float process() override; // Returns envelope value 0.0-1.0
        float process(float input) override; // Applies envelope to input
        void reset() override;
        void setSampleRate(uint32_t sampleRate) override;
        
    private:
        float attackTime, decayTime, sustainLevel, releaseTime;
        EnvelopeStage stage;
        float currentLevel;
        float targetLevel;
        float rate;
        uint32_t sampleRate;
    };
}
```

## Hardware Integration

### PWM Audio Output

```cpp
namespace KoeKit {
    class PWMOutput {
    public:
        static bool init(uint8_t pin = 1, uint32_t sampleRate = 22050, uint8_t resolution = 10);
        static void write(float sample); // Input: -1.0 to 1.0
        static void write(int16_t sample); // Input: 16-bit signed
        static void shutdown();
        
    private:
        static uint8_t outputPin;
        static uint32_t sampleRate;
        static uint8_t resolution;
        static uint16_t maxValue;
    };
}
```

### ADC Input

```cpp
namespace KoeKit {
    class ADCInput {
    public:
        ADCInput(uint8_t pin);
        
        float read(); // Returns 0.0 to 1.0
        float readScaled(float min, float max); // Returns scaled value
        void setSmoothing(float factor); // Low-pass filtering
        
    private:
        uint8_t adcPin;
        float smoothingFactor;
        float lastValue;
    };
}
```

## Utility Classes

### Digital Input

```cpp
namespace KoeKit {
    class DigitalInput {
    public:
        DigitalInput(uint8_t pin, bool pullup = true);
        
        bool read();
        bool readRising(); // True on rising edge
        bool readFalling(); // True on falling edge
        
    private:
        uint8_t digitalPin;
        bool lastState;
        bool pullupEnabled;
    };
}
```

### Math Utilities

```cpp
namespace KoeKit {
    namespace Math {
        float map(float value, float inMin, float inMax, float outMin, float outMax);
        float constrain(float value, float min, float max);
        float lerp(float a, float b, float t);
        
        // Fast approximations
        float fastSin(float x);
        float fastCos(float x);
        float fastExp(float x);
        float fastLog(float x);
    }
}
```

## Platform-Specific Features

### RP2040/2350 Optimizations

```cpp
namespace KoeKit {
    namespace Platform {
        // Core utilization
        void enableDualCore(bool enable = true);
        void setAudioCore(uint8_t core = 1); // 0 or 1
        
        // DMA optimizations
        void enableDMAOutput(bool enable = true);
        void setDMABufferSize(uint16_t size = 256);
        
        // Clock optimizations
        void setSystemClock(uint32_t frequency = 133000000);
        void enableOverclock(bool enable = false);
    }
}
```

## Example Usage

### Basic Audio Chain

```cpp
#include "KoeKit.h"

KoeKit::AudioEngine engine;
KoeKit::WavetableOsc osc;
KoeKit::Filter filter;
KoeKit::ADSR envelope;
KoeKit::PWMOutput output;
KoeKit::ADCInput freqControl(A0);

void setup() {
    // Initialize audio engine
    KoeKit::AudioEngine::init(22050, 64);
    
    // Configure oscillator
    osc.setWaveform(KoeKit::SINE);
    osc.setFrequency(440.0);
    
    // Configure filter
    filter.setType(KoeKit::LOWPASS);
    filter.setCutoff(2000.0);
    filter.setResonance(0.3);
    
    // Configure envelope
    envelope.setADSR(0.1, 0.2, 0.7, 0.5);
    
    // Initialize PWM output
    KoeKit::PWMOutput::init(1, 22050, 10);
}

void loop() {
    // Read frequency control
    float freqCV = freqControl.readScaled(220.0, 880.0);
    osc.setFrequency(freqCV);
    
    // Process audio chain
    float sample = osc.process();
    sample = filter.process(sample);
    sample = envelope.process(sample);
    
    // Output to PWM
    KoeKit::PWMOutput::write(sample);
    
    delayMicroseconds(45); // ~22kHz timing
}
```

## Performance Specifications

- **Sample Rate**: Up to 44.1kHz (22kHz recommended)
- **Latency**: < 3ms at 22kHz
- **CPU Usage**: < 30% at 133MHz
- **Memory**: < 16KB RAM usage
- **Polyphony**: 4-8 voices depending on complexity

## Dependencies

- **Arduino-pico**: >= 3.0.0
- **Hardware**: RP2040 or RP2350 based boards
- **RAM**: Minimum 32KB
- **Flash**: Minimum 128KB

## Installation

```bash
# Arduino Library Manager
# Search for "KoeKit" and install

# Or manual installation
git clone https://github.com/username/KoeKit.git
cp -r KoeKit ~/Arduino/libraries/
```

## License

MIT License - See LICENSE file for details.
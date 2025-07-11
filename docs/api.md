# KoeKit API Reference

Complete API reference for KoeKit audio synthesis library.

## Table of Contents

- [Core Classes](#core-classes)
  - [AudioEngine](#audioengine)
  - [Oscillator](#oscillator)
  - [NoiseGenerator](#noisegenerator)
- [Wavetables](#wavetables)
  - [Wavetable](#wavetable)
  - [Basic Waveforms](#basic-waveforms)
  - [Custom Wavetables](#custom-wavetables)
- [Filters](#filters)
  - [OnePole](#onepole)
  - [StateVariable](#statevariable)
  - [Biquad](#biquad)
  - [DCBlocker](#dcblocker)
- [Envelopes](#envelopes)
  - [ADSR](#adsr)
  - [AR](#ar)
  - [LFO](#lfo)
- [Global Functions](#global-functions)

## Core Classes

### AudioEngine

Main audio engine class that manages the entire audio system.

```cpp
class AudioEngine
```

#### Methods

##### `begin()`
```cpp
bool begin(uint32_t sample_rate = SAMPLE_RATE, uint8_t output_pin = 1)
```
Initialize the audio engine.

**Parameters:**
- `sample_rate`: Sample rate in Hz (default: 22050)
- `output_pin`: PWM output pin (default: 1)

**Returns:** `true` if initialization successful

**Example:**
```cpp
if (!KoeKit::begin(22050, 1)) {
  Serial.println("Failed to initialize KoeKit!");
}
```

##### `setAudioCallback()`
```cpp
void setAudioCallback(AudioCallback callback)
```
Set the audio processing callback function.

**Parameters:**
- `callback`: Function that returns `float` audio samples

**Example:**
```cpp
KoeKit::setAudioCallback([]() -> float {
  return oscillator.process();
});
```

##### `end()`
```cpp
void end()
```
Stop the audio engine and release resources.

##### `getSampleRate()`
```cpp
uint32_t getSampleRate()
```
Get the current sample rate.

**Returns:** Sample rate in Hz

---

### Oscillator

High-quality wavetable oscillator with linear interpolation.

```cpp
template<size_t TABLE_SIZE>
class WavetableOscillator
using Oscillator = WavetableOscillator<WAVETABLE_SIZE>;
```

#### Constructor

```cpp
Oscillator(const Wavetable<TABLE_SIZE>& wavetable)
```

**Example:**
```cpp
KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);
```

#### Methods

##### `setFrequency()`
```cpp
void setFrequency(float frequency)
```
Set oscillator frequency.

**Parameters:**
- `frequency`: Frequency in Hz

##### `setAmplitude()`
```cpp
void setAmplitude(float amplitude)
```
Set oscillator amplitude.

**Parameters:**
- `amplitude`: Amplitude (0.0 to 1.0)

##### `process()`
```cpp
float process()
```
Generate one audio sample.

**Returns:** Audio sample (-1.0 to 1.0)

##### `reset()`
```cpp
void reset()
```
Reset oscillator phase to zero.

##### `setWavetable()`
```cpp
void setWavetable(const Wavetable<TABLE_SIZE>& wavetable)
```
Change the oscillator's wavetable.

**Example:**
```cpp
osc.setFrequency(440.0f);
osc.setAmplitude(0.5f);
float sample = osc.process();
```

---

### NoiseGenerator

Fast pseudo-random noise generator using XorShift algorithm.

```cpp
class NoiseGenerator
```

#### Methods

##### `setAmplitude()`
```cpp
void setAmplitude(float amplitude)
```
Set noise amplitude (0.0 to 1.0).

##### `process()`
```cpp
float process()
```
Generate one noise sample (-1.0 to 1.0).

##### `reset()`
```cpp
void reset(uint32_t seed = 1)
```
Reset with new random seed.

**Example:**
```cpp
KoeKit::NoiseGenerator noise;
noise.setAmplitude(0.3f);
float sample = noise.process();
```

---

## Wavetables

### Wavetable

Container for wavetable data with interpolation support.

```cpp
template<size_t SIZE>
class Wavetable
```

#### Methods

##### `getSample()`
```cpp
WavetableSample getSample(size_t index) const
```
Get sample at exact index (no interpolation).

##### `getInterpolated()`
```cpp
float getInterpolated(float index) const
```
Get interpolated sample at fractional index.

##### `size()`
```cpp
constexpr size_t size() const
```
Get table size.

---

### Basic Waveforms

Pre-defined waveforms available in `KoeKit::Wavetables::Basic`:

#### Available Waveforms

```cpp
enum class Waveform : uint8_t {
    SINE = 0,
    SAW = 1,
    SQUARE = 2,
    TRIANGLE = 3,
    SOFT_SAW = 4,
    PULSE = 5
};
```

#### Pre-computed Tables

```cpp
inline constexpr auto SINE = makeSineTable();
inline constexpr auto SAW = makeSawTable();
inline constexpr auto SQUARE = makeSquareTable();
inline constexpr auto TRIANGLE = makeTriangleTable();
inline constexpr auto SOFT_SAW = makeSoftSawTable();
inline constexpr auto PULSE = makePulseTable();
```

#### `getWavetable()`
```cpp
constexpr const auto& getWavetable(Waveform waveform)
```
Get wavetable by waveform type.

**Example:**
```cpp
auto osc = KoeKit::createOscillator(KoeKit::Wavetables::Basic::Waveform::SINE);

// Or direct access
KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);
```

---

### Custom Wavetables

#### `makeWavetable()` (Formula-based)
```cpp
template<size_t SIZE, typename Generator>
constexpr auto makeWavetable(Generator generator)
```
Generate wavetable from mathematical formula.

**Example:**
```cpp
constexpr auto myWave = KoeKit::makeWavetable<1024>([](size_t i) -> float {
  float phase = 2.0f * M_PI * static_cast<float>(i) / 1024.0f;
  return std::sin(phase) + 0.3f * std::sin(3.0f * phase);
});
```

#### `makeWavetable()` (Sample-based)
```cpp
template<size_t SIZE>
constexpr auto makeWavetable(const std::array<float, SIZE>& samples)
```
Generate wavetable from sample array.

**Example:**
```cpp
std::array<float, 1024> samples = { /* your samples */ };
constexpr auto customWave = KoeKit::makeWavetable(samples);
```

---

## Filters

### OnePole

Simple and efficient first-order filter.

```cpp
class OnePole
```

#### Methods

##### `setCutoff()`
```cpp
void setCutoff(float cutoff)
```
Set cutoff frequency in Hz.

##### `processLPF()`
```cpp
float processLPF(float input)
```
Process as low-pass filter.

##### `processHPF()`
```cpp
float processHPF(float input)
```
Process as high-pass filter.

**Example:**
```cpp
KoeKit::Filter::OnePole filter;
filter.setCutoff(1000.0f);
float filtered = filter.processLPF(input);
```

---

### StateVariable

High-quality state variable filter with simultaneous outputs.

```cpp
class StateVariable
```

#### Methods

##### `setParams()`
```cpp
void setParams(float cutoff, float resonance)
```
Set filter parameters.

**Parameters:**
- `cutoff`: Cutoff frequency in Hz
- `resonance`: Resonance factor (0.1 to 10.0)

##### `process()`
```cpp
void process(float input)
```
Process input sample through filter.

##### Output Methods
```cpp
float getLowPass() const    // Low-pass output
float getHighPass() const   // High-pass output
float getBandPass() const   // Band-pass output
float getNotch() const      // Notch output
```

**Example:**
```cpp
KoeKit::Filter::StateVariable filter;
filter.setParams(800.0f, 2.0f);
filter.process(input);
float output = filter.getLowPass();
```

---

### Biquad

General-purpose biquad filter implementation.

```cpp
class Biquad
```

#### Methods

##### `setLowPass()`
```cpp
void setLowPass(float cutoff)
```
Configure as Butterworth low-pass filter.

##### `setHighPass()`
```cpp
void setHighPass(float cutoff)
```
Configure as Butterworth high-pass filter.

##### `setBandPass()`
```cpp
void setBandPass(float center, float bandwidth)
```
Configure as band-pass filter.

##### `process()`
```cpp
float process(float input)
```
Process input sample.

**Example:**
```cpp
KoeKit::Filter::Biquad filter;
filter.setLowPass(1000.0f);
float output = filter.process(input);
```

---

### DCBlocker

Simple DC offset removal filter.

```cpp
class DCBlocker
```

#### Methods

##### `process()`
```cpp
float process(float input)
```
Remove DC offset from input.

**Example:**
```cpp
KoeKit::Filter::DCBlocker dcblock;
float output = dcblock.process(input);
```

---

## Envelopes

### ADSR

Classic Attack-Decay-Sustain-Release envelope generator.

```cpp
class ADSR
```

#### Enums

```cpp
enum class Stage : uint8_t {
    IDLE, ATTACK, DECAY, SUSTAIN, RELEASE
};
```

#### Methods

##### `setADSR()`
```cpp
void setADSR(float attack, float decay, float sustain, float release)
```
Set all ADSR parameters.

**Parameters:**
- `attack`: Attack time in seconds
- `decay`: Decay time in seconds
- `sustain`: Sustain level (0.0 to 1.0)
- `release`: Release time in seconds

##### `noteOn()` / `noteOff()`
```cpp
void noteOn()    // Start envelope
void noteOff()   // Begin release phase
```

##### `process()`
```cpp
float process()                // Get envelope level
float process(float input)     // Apply envelope to input
```

##### `isActive()`
```cpp
bool isActive() const
```
Check if envelope is generating output.

**Example:**
```cpp
KoeKit::Envelope::ADSR env;
env.setADSR(0.01f, 0.1f, 0.7f, 0.3f);
env.noteOn();

float envLevel = env.process();
float output = oscillator.process() * envLevel;
```

---

### AR

Simple Attack-Release envelope for percussive sounds.

```cpp
class AR
```

#### Methods

##### `setAR()`
```cpp
void setAR(float attack, float release)
```
Set attack and release times.

##### `trigger()`
```cpp
void trigger()
```
Trigger the envelope.

##### `process()`
```cpp
float process()
float process(float input)
```

**Example:**
```cpp
KoeKit::Envelope::AR env;
env.setAR(0.001f, 0.5f);
env.trigger();
```

---

### LFO

Low Frequency Oscillator for parameter modulation.

```cpp
class LFO
```

#### Enums

```cpp
enum class Waveform : uint8_t {
    SINE, TRIANGLE, SAWTOOTH, SQUARE, SAMPLE_HOLD, NOISE
};
```

#### Methods

##### `setFrequency()`
```cpp
void setFrequency(float frequency)
```
Set LFO frequency (typically 0.1 to 20 Hz).

##### `setAmplitude()`
```cpp
void setAmplitude(float amplitude)
```
Set LFO amplitude (0.0 to 1.0).

##### `setOffset()`
```cpp
void setOffset(float offset)
```
Set DC offset (-1.0 to 1.0).

##### `setWaveform()`
```cpp
void setWaveform(Waveform waveform)
```
Set LFO waveform type.

##### `process()`
```cpp
float process()
```
Generate LFO output.

**Example:**
```cpp
KoeKit::Envelope::LFO lfo;
lfo.setFrequency(2.0f);
lfo.setWaveform(KoeKit::Envelope::LFO::Waveform::SINE);
lfo.setAmplitude(0.5f);

float modulation = lfo.process();
oscillator.setFrequency(440.0f + modulation * 100.0f);
```

---

## Global Functions

### Initialization

```cpp
bool begin(uint32_t sample_rate = SAMPLE_RATE, uint8_t output_pin = 1)
void end()
uint32_t getSampleRate()
```

### Audio Callback

```cpp
void setAudioCallback(AudioCallback callback)
```

### Utility Functions

```cpp
Oscillator createOscillator(Wavetables::Basic::Waveform waveform)
```
Create oscillator with basic waveform.

---

## Usage Patterns

### Basic Synthesis Chain

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);
KoeKit::Filter::OnePole filter;
KoeKit::Envelope::ADSR env;

void setup() {
  KoeKit::begin(22050, 1);
  
  osc.setFrequency(440.0f);
  filter.setCutoff(1000.0f);
  env.setADSR(0.01f, 0.1f, 0.7f, 0.3f);
  
  KoeKit::setAudioCallback([]() -> float {
    float sample = osc.process();
    sample = filter.processLPF(sample);
    sample = env.process(sample);
    return sample;
  });
  
  env.noteOn();
}
```

### Multiple Oscillators

```cpp
KoeKit::Oscillator osc1(KoeKit::Wavetables::Basic::SINE);
KoeKit::Oscillator osc2(KoeKit::Wavetables::Basic::SAW);

KoeKit::setAudioCallback([]() -> float {
  float mix = osc1.process() * 0.5f + osc2.process() * 0.5f;
  return mix;
});
```

### LFO Modulation

```cpp
KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);
KoeKit::Envelope::LFO lfo;

void setup() {
  lfo.setFrequency(2.0f);
  lfo.setAmplitude(0.2f);
  
  KoeKit::setAudioCallback([]() -> float {
    float mod = lfo.process();
    osc.setFrequency(440.0f * (1.0f + mod));
    return osc.process();
  });
}
```

---

## Performance Guidelines

### Memory Usage

- Basic wavetable (1024 samples): 2KB
- Each oscillator instance: ~50 bytes
- Each filter instance: ~20 bytes
- Each envelope instance: ~30 bytes

### CPU Usage (at 22kHz)

- Oscillator: ~1% per instance
- OnePole filter: ~0.5% per instance  
- StateVariable filter: ~1% per instance
- ADSR envelope: ~0.5% per instance

### Best Practices

1. **Pre-allocate objects** in global scope when possible
2. **Use OnePole filters** for simple filtering tasks
3. **Limit active voices** based on target CPU usage
4. **Use constexpr wavetables** for compile-time optimization
5. **Avoid dynamic memory allocation** in audio callback

---

## Error Handling

KoeKit uses safe parameter clamping rather than exceptions:

- Frequencies are clamped to valid ranges
- Amplitudes are clamped to 0.0-1.0
- Filter parameters are constrained to stable values

Always check the return value of `KoeKit::begin()` for initialization errors.
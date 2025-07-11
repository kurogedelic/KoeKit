# KoeKit Examples

This document provides detailed explanations and advanced examples for KoeKit audio synthesis.

## Table of Contents

1. [Basic Examples](#basic-examples)
2. [Filter Examples](#filter-examples)
3. [Envelope Examples](#envelope-examples)
4. [Custom Wavetables](#custom-wavetables)
5. [Complete Synthesizers](#complete-synthesizers)
6. [Performance Optimization](#performance-optimization)

## Basic Examples

### Simple Tone Generator

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);

void setup() {
  KoeKit::begin(22050, 1);
  
  osc.setFrequency(440.0f);
  osc.setAmplitude(0.3f);
  
  KoeKit::setAudioCallback([]() -> float {
    return osc.process();
  });
}

void loop() {
  // Audio runs automatically in background
}
```

### Musical Scale Player

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);

// C Major scale frequencies
float scale[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
int currentNote = 0;
unsigned long lastNoteTime = 0;

void setup() {
  KoeKit::begin(22050, 1);
  osc.setAmplitude(0.4f);
  
  KoeKit::setAudioCallback([]() -> float {
    return osc.process();
  });
}

void loop() {
  if (millis() - lastNoteTime > 500) {  // Change note every 500ms
    osc.setFrequency(scale[currentNote]);
    currentNote = (currentNote + 1) % 8;
    lastNoteTime = millis();
  }
}
```

### Dual Oscillator Detune

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc1(KoeKit::Wavetables::Basic::SAW);
KoeKit::Oscillator osc2(KoeKit::Wavetables::Basic::SAW);

void setup() {
  KoeKit::begin(22050, 1);
  
  osc1.setFrequency(440.0f);      // Base frequency
  osc2.setFrequency(442.0f);      // Slightly detuned for beat effect
  osc1.setAmplitude(0.3f);
  osc2.setAmplitude(0.3f);
  
  KoeKit::setAudioCallback([]() -> float {
    return osc1.process() + osc2.process();
  });
}

void loop() {}
```

## Filter Examples

### Low-Pass Filtered Sawtooth

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SAW);
KoeKit::Filter::OnePole filter;

void setup() {
  KoeKit::begin(22050, 1);
  
  osc.setFrequency(220.0f);
  osc.setAmplitude(0.5f);
  filter.setCutoff(800.0f);  // 800Hz cutoff
  
  KoeKit::setAudioCallback([]() -> float {
    float sample = osc.process();
    return filter.processLPF(sample);
  });
}

void loop() {}
```

### Resonant Filter Sweep

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SAW);
KoeKit::Filter::StateVariable filter;

float cutoffFreq = 200.0f;
bool sweepingUp = true;

void setup() {
  KoeKit::begin(22050, 1);
  
  osc.setFrequency(110.0f);  // Low bass note
  osc.setAmplitude(0.4f);
  
  KoeKit::setAudioCallback([]() -> float {
    // Sweep filter cutoff
    if (sweepingUp) {
      cutoffFreq += 10.0f;
      if (cutoffFreq >= 2000.0f) sweepingUp = false;
    } else {
      cutoffFreq -= 10.0f;
      if (cutoffFreq <= 200.0f) sweepingUp = true;
    }
    
    filter.setParams(cutoffFreq, 3.0f);  // High resonance
    
    float sample = osc.process();
    filter.process(sample);
    return filter.getLowPass();
  });
}

void loop() {}
```

### Multi-Mode Filter

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SAW);
KoeKit::Filter::StateVariable filter;

int filterMode = 0;  // 0=LP, 1=HP, 2=BP, 3=Notch
unsigned long lastModeChange = 0;

void setup() {
  KoeKit::begin(22050, 1);
  
  osc.setFrequency(440.0f);
  osc.setAmplitude(0.4f);
  filter.setParams(1000.0f, 2.0f);
  
  KoeKit::setAudioCallback([]() -> float {
    float sample = osc.process();
    filter.process(sample);
    
    switch (filterMode) {
      case 0: return filter.getLowPass();
      case 1: return filter.getHighPass();
      case 2: return filter.getBandPass();
      case 3: return filter.getNotch();
      default: return sample;
    }
  });
}

void loop() {
  if (millis() - lastModeChange > 2000) {  // Change mode every 2 seconds
    filterMode = (filterMode + 1) % 4;
    lastModeChange = millis();
    
    const char* modes[] = {"Low-Pass", "High-Pass", "Band-Pass", "Notch"};
    Serial.println(modes[filterMode]);
  }
}
```

## Envelope Examples

### Basic ADSR Envelope

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SAW);
KoeKit::Envelope::ADSR env;

const int TRIGGER_PIN = 2;
bool lastButtonState = HIGH;

void setup() {
  KoeKit::begin(22050, 1);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  
  osc.setFrequency(440.0f);
  osc.setAmplitude(0.5f);
  
  // Set ADSR: Attack, Decay, Sustain, Release
  env.setADSR(0.01f, 0.2f, 0.6f, 0.5f);
  
  KoeKit::setAudioCallback([]() -> float {
    float sample = osc.process();
    return env.process(sample);  // Apply envelope
  });
}

void loop() {
  bool currentButtonState = digitalRead(TRIGGER_PIN);
  
  // Trigger on button press
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    env.noteOn();
  }
  // Release on button release
  else if (lastButtonState == LOW && currentButtonState == HIGH) {
    env.noteOff();
  }
  
  lastButtonState = currentButtonState;
  delay(10);
}
```

### Percussive AR Envelope

```cpp
#include <KoeKit.h>

KoeKit::NoiseGenerator noise;
KoeKit::Envelope::AR env;
KoeKit::Filter::OnePole filter;

const int TRIGGER_PIN = 2;
bool lastButtonState = HIGH;

void setup() {
  KoeKit::begin(22050, 1);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  
  noise.setAmplitude(0.8f);
  env.setAR(0.001f, 0.3f);  // Very fast attack, medium release
  filter.setCutoff(2000.0f);  // High-pass for snare-like sound
  
  KoeKit::setAudioCallback([]() -> float {
    float sample = noise.process();
    sample = filter.processHPF(sample);
    return env.process(sample);
  });
}

void loop() {
  bool currentButtonState = digitalRead(TRIGGER_PIN);
  
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    env.trigger();  // Trigger percussive envelope
  }
  
  lastButtonState = currentButtonState;
  delay(10);
}
```

### LFO Modulation

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);
KoeKit::Envelope::LFO lfo;

void setup() {
  KoeKit::begin(22050, 1);
  
  osc.setFrequency(440.0f);
  osc.setAmplitude(0.4f);
  
  lfo.setFrequency(2.0f);  // 2Hz LFO
  lfo.setAmplitude(0.3f);  // 30% modulation depth
  lfo.setWaveform(KoeKit::Envelope::LFO::Waveform::SINE);
  
  KoeKit::setAudioCallback([]() -> float {
    float modulation = lfo.process();
    
    // Frequency modulation (vibrato)
    osc.setFrequency(440.0f * (1.0f + modulation));
    
    return osc.process();
  });
}

void loop() {}
```

## Custom Wavetables

### Harmonic Series Generator

```cpp
#include <KoeKit.h>

// Generate wavetable with specific harmonic content
constexpr auto harmonicWave = KoeKit::makeWavetable<1024>([](size_t i) -> float {
  float phase = 2.0f * M_PI * static_cast<float>(i) / 1024.0f;
  
  // Add multiple harmonics with decreasing amplitude
  float output = 0.0f;
  output += 0.5f * sin(1.0f * phase);  // Fundamental
  output += 0.3f * sin(2.0f * phase);  // 2nd harmonic
  output += 0.2f * sin(3.0f * phase);  // 3rd harmonic
  output += 0.1f * sin(4.0f * phase);  // 4th harmonic
  output += 0.05f * sin(5.0f * phase); // 5th harmonic
  
  return output * 0.5f;  // Scale to prevent clipping
});

KoeKit::Oscillator osc(harmonicWave);

void setup() {
  KoeKit::begin(22050, 1);
  
  osc.setFrequency(220.0f);
  osc.setAmplitude(0.4f);
  
  KoeKit::setAudioCallback([]() -> float {
    return osc.process();
  });
}

void loop() {}
```

### Morphing Wavetables

```cpp
#include <KoeKit.h>

// Create multiple wavetables
constexpr auto wave1 = KoeKit::makeWavetable<1024>([](size_t i) -> float {
  float phase = 2.0f * M_PI * static_cast<float>(i) / 1024.0f;
  return sin(phase);  // Sine
});

constexpr auto wave2 = KoeKit::makeWavetable<1024>([](size_t i) -> float {
  return 2.0f * static_cast<float>(i) / 1023.0f - 1.0f;  // Sawtooth
});

KoeKit::Oscillator osc1(wave1);
KoeKit::Oscillator osc2(wave2);
KoeKit::Envelope::LFO morphLFO;

void setup() {
  KoeKit::begin(22050, 1);
  
  osc1.setFrequency(220.0f);
  osc2.setFrequency(220.0f);
  
  morphLFO.setFrequency(0.5f);  // Slow morph
  morphLFO.setAmplitude(1.0f);
  morphLFO.setOffset(0.0f);
  
  KoeKit::setAudioCallback([]() -> float {
    float morphAmount = (morphLFO.process() + 1.0f) * 0.5f;  // 0.0 to 1.0
    
    float sample1 = osc1.process();
    float sample2 = osc2.process();
    
    // Crossfade between waveforms
    return sample1 * (1.0f - morphAmount) + sample2 * morphAmount;
  });
}

void loop() {}
```

### Wavetable from Samples

```cpp
#include <KoeKit.h>

// Create wavetable from explicit sample values
constexpr auto customSamples = []() {
  std::array<float, 1024> samples{};
  
  // Create a "stepped" waveform
  for (size_t i = 0; i < 1024; ++i) {
    float t = static_cast<float>(i) / 1024.0f;
    
    // Quantize to 8 levels
    float level = floor(t * 8.0f) / 8.0f;
    samples[i] = level * 2.0f - 1.0f;  // Scale to -1 to 1
  }
  
  return samples;
}();

constexpr auto steppedWave = KoeKit::makeWavetable(customSamples);

KoeKit::Oscillator osc(steppedWave);

void setup() {
  KoeKit::begin(22050, 1);
  
  osc.setFrequency(110.0f);
  osc.setAmplitude(0.4f);
  
  KoeKit::setAudioCallback([]() -> float {
    return osc.process();
  });
}

void loop() {}
```

## Complete Synthesizers

### Monophonic Synthesizer

```cpp
#include <KoeKit.h>

class MonoSynth {
private:
  KoeKit::Oscillator osc1;
  KoeKit::Oscillator osc2;
  KoeKit::Filter::StateVariable filter;
  KoeKit::Envelope::ADSR ampEnv;
  KoeKit::Envelope::ADSR filterEnv;
  KoeKit::Envelope::LFO lfo;
  
  float baseFreq = 440.0f;
  
public:
  MonoSynth() : 
    osc1(KoeKit::Wavetables::Basic::SAW),
    osc2(KoeKit::Wavetables::Basic::SQUARE) {
    
    // Setup oscillators
    osc1.setAmplitude(0.6f);
    osc2.setAmplitude(0.4f);
    
    // Setup filter
    filter.setParams(1000.0f, 2.0f);
    
    // Setup envelopes
    ampEnv.setADSR(0.01f, 0.3f, 0.7f, 0.5f);
    filterEnv.setADSR(0.01f, 0.5f, 0.3f, 0.8f);
    
    // Setup LFO
    lfo.setFrequency(5.0f);
    lfo.setAmplitude(0.1f);
  }
  
  void setFrequency(float freq) {
    baseFreq = freq;
    updateFrequencies();
  }
  
  void noteOn() {
    ampEnv.noteOn();
    filterEnv.noteOn();
  }
  
  void noteOff() {
    ampEnv.noteOff();
    filterEnv.noteOff();
  }
  
  float process() {
    // LFO modulation
    float lfoValue = lfo.process();
    
    // Update oscillator frequencies with LFO
    osc1.setFrequency(baseFreq * (1.0f + lfoValue));
    osc2.setFrequency(baseFreq * 1.005f * (1.0f + lfoValue));  // Slight detune
    
    // Mix oscillators
    float oscMix = osc1.process() + osc2.process();
    
    // Apply filter with envelope modulation
    float filterMod = filterEnv.process();
    filter.setParams(1000.0f + filterMod * 2000.0f, 2.0f);
    filter.process(oscMix);
    float filtered = filter.getLowPass();
    
    // Apply amplitude envelope
    return ampEnv.process(filtered);
  }
  
private:
  void updateFrequencies() {
    osc1.setFrequency(baseFreq);
    osc2.setFrequency(baseFreq * 1.005f);  // Slight detune
  }
};

MonoSynth synth;
const int TRIGGER_PIN = 2;
bool lastButtonState = HIGH;

void setup() {
  KoeKit::begin(22050, 1);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  
  synth.setFrequency(220.0f);
  
  KoeKit::setAudioCallback([]() -> float {
    return synth.process();
  });
}

void loop() {
  bool currentButtonState = digitalRead(TRIGGER_PIN);
  
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    synth.noteOn();
  } else if (lastButtonState == LOW && currentButtonState == HIGH) {
    synth.noteOff();
  }
  
  lastButtonState = currentButtonState;
  delay(10);
}
```

### Simple Drum Machine

```cpp
#include <KoeKit.h>

class DrumMachine {
private:
  // Kick drum
  KoeKit::Oscillator kickOsc;
  KoeKit::Envelope::AR kickEnv;
  KoeKit::Envelope::LFO kickPitch;
  
  // Snare drum
  KoeKit::NoiseGenerator snareNoise;
  KoeKit::Envelope::AR snareEnv;
  KoeKit::Filter::Biquad snareFilter;
  
  // Hi-hat
  KoeKit::NoiseGenerator hatNoise;
  KoeKit::Envelope::AR hatEnv;
  KoeKit::Filter::Biquad hatFilter;
  
public:
  DrumMachine() : kickOsc(KoeKit::Wavetables::Basic::SINE) {
    // Setup kick
    kickOsc.setAmplitude(0.8f);
    kickEnv.setAR(0.001f, 0.3f);
    kickPitch.setFrequency(50.0f);
    kickPitch.setAmplitude(0.8f);
    kickPitch.setOffset(-0.8f);
    
    // Setup snare
    snareNoise.setAmplitude(0.6f);
    snareEnv.setAR(0.001f, 0.15f);
    snareFilter.setBandPass(800.0f, 400.0f);
    
    // Setup hi-hat
    hatNoise.setAmplitude(0.4f);
    hatEnv.setAR(0.001f, 0.05f);
    hatFilter.setHighPass(8000.0f);
  }
  
  void triggerKick() {
    kickEnv.trigger();
  }
  
  void triggerSnare() {
    snareEnv.trigger();
  }
  
  void triggerHat() {
    hatEnv.trigger();
  }
  
  float process() {
    float output = 0.0f;
    
    // Kick drum with pitch envelope
    float pitchMod = kickPitch.process();
    kickOsc.setFrequency(60.0f * (1.0f + pitchMod));
    output += kickEnv.process(kickOsc.process()) * 0.8f;
    
    // Snare drum
    float snare = snareNoise.process();
    snare = snareFilter.process(snare);
    output += snareEnv.process(snare) * 0.6f;
    
    // Hi-hat
    float hat = hatNoise.process();
    hat = hatFilter.process(hat);
    output += hatEnv.process(hat) * 0.4f;
    
    return output;
  }
};

DrumMachine drums;
unsigned long lastBeat = 0;
int beatCount = 0;
const int BPM = 120;
const int BEAT_INTERVAL = 60000 / (BPM * 4);  // 16th notes

void setup() {
  KoeKit::begin(22050, 1);
  
  KoeKit::setAudioCallback([]() -> float {
    return drums.process();
  });
}

void loop() {
  if (millis() - lastBeat > BEAT_INTERVAL) {
    // Simple drum pattern
    switch (beatCount % 16) {
      case 0: case 8:
        drums.triggerKick();
        break;
      case 4: case 12:
        drums.triggerSnare();
        break;
      case 2: case 6: case 10: case 14:
        drums.triggerHat();
        break;
    }
    
    beatCount++;
    lastBeat = millis();
  }
}
```

## Performance Optimization

### Efficient Mixing

```cpp
// Instead of this (inefficient):
float sample = osc1.process() * 0.5f + osc2.process() * 0.5f;

// Do this (more efficient):
static float mix = 0.0f;
mix = osc1.process();
mix += osc2.process();
mix *= 0.5f;
```

### Lookup Table Optimization

```cpp
// Pre-calculate expensive operations
constexpr float FREQ_TO_INCREMENT = 1.0f / 22050.0f;

// In audio callback:
float increment = frequency * FREQ_TO_INCREMENT;  // Multiply instead of divide
```

### Memory-Efficient Voice Management

```cpp
template<size_t NumVoices>
class VoiceManager {
private:
  std::array<KoeKit::Oscillator, NumVoices> voices;
  std::array<KoeKit::Envelope::ADSR, NumVoices> envelopes;
  std::array<bool, NumVoices> voiceActive;
  
public:
  float process() {
    float mix = 0.0f;
    
    for (size_t i = 0; i < NumVoices; ++i) {
      if (voiceActive[i]) {
        float sample = voices[i].process();
        sample = envelopes[i].process(sample);
        mix += sample;
        
        // Deactivate voice when envelope is done
        if (!envelopes[i].isActive()) {
          voiceActive[i] = false;
        }
      }
    }
    
    return mix / NumVoices;  // Normalize
  }
  
  void noteOn(float frequency) {
    // Find inactive voice
    for (size_t i = 0; i < NumVoices; ++i) {
      if (!voiceActive[i]) {
        voices[i].setFrequency(frequency);
        envelopes[i].noteOn();
        voiceActive[i] = true;
        break;
      }
    }
  }
};
```

These examples demonstrate the flexibility and power of KoeKit for creating complex audio applications on the RP2350A platform.
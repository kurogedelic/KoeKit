# Getting Started with KoeKit

This guide will help you get up and running with KoeKit audio synthesis library on your RP2350A board.

::: tip What You'll Learn
By the end of this guide, you'll have KoeKit installed and generating your first audio on an RP2350A board.
:::

## Overview

KoeKit is designed to make high-quality audio synthesis accessible on RP2350A microcontrollers. This guide covers everything from installation to your first synthesized sound.

## Hardware Requirements

### Supported Boards
- **Raspberry Pi Pico 2** (RP2350A)
- Any RP2350A-based development board
- **Note**: Currently optimized for RP2350A only

### Audio Output Circuit

#### Basic Setup (Minimal)
```
RP2350A Pin 1 → [1kΩ resistor] → [Speaker/Headphones] → GND
```

#### Recommended Setup (Better Quality)
```
RP2350A Pin 1 → [1kΩ resistor] → [100nF capacitor] → [Speaker/Headphones] → GND
              ↘ [100nF capacitor] → GND
```

#### Professional Setup (Best Quality)
```
RP2350A Pin 1 → [RC Low-Pass Filter] → [Audio Amplifier] → [Speaker]
```

**RC Filter Values:**
- R = 1kΩ
- C = 100nF  
- Cutoff ≈ 1.6kHz (removes PWM artifacts)

### Power Considerations
- USB power is sufficient for most applications
- For high-volume applications, consider external power supply
- Current draw: ~50mA typical

## Installation

### Prerequisites
1. **Arduino IDE** (1.8.19 or later) or **Arduino CLI**
2. **arduino-pico board package** (4.0.0 or later)

### Install Board Package

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add this URL to "Additional Board Manager URLs":
   ```
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```
4. Go to **Tools → Board → Board Manager**
5. Search for "pico" and install **"Raspberry Pi Pico/RP2040/RP2350"**

### Install KoeKit Library

#### Method 1: Arduino Library Manager (Future)
*Will be available when published to Arduino Library Registry*

#### Method 2: Manual Installation
1. Download KoeKit from GitHub
2. Extract to your Arduino libraries folder:
   - **Windows**: `Documents\Arduino\libraries\KoeKit\`
   - **macOS**: `~/Documents/Arduino/libraries/KoeKit/`
   - **Linux**: `~/Arduino/libraries/KoeKit/`
3. Restart Arduino IDE

#### Method 3: Git Clone
```bash
cd ~/Documents/Arduino/libraries/
git clone https://github.com/username/KoeKit.git
```

### Board Configuration

Select your board in Arduino IDE:
- **Tools → Board → Raspberry Pi Pico/RP2040/RP2350 → Raspberry Pi Pico 2**

Recommended settings:
- **CPU Speed**: 150MHz (default)
- **Flash Size**: 4MB (default)
- **Boot Stage 2**: Generic SPI /4 (default)

## Basic Setup

### Include the Library

```cpp
#include <KoeKit.h>
```

This single include gives you access to all KoeKit functionality.

### Initialize Audio System

```cpp
void setup() {
  Serial.begin(115200);
  
  // Initialize KoeKit
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("KoeKit initialization failed!");
    while(1); // Stop execution
  }
  
  Serial.println("KoeKit ready!");
}
```

**Parameters:**
- `22050`: Sample rate (Hz) - 22kHz recommended for RP2350A
- `1`: Output pin - GPIO pin for PWM audio output

## Your First Sound

Let's create a simple 440Hz sine wave (A4 note):

```cpp
#include <KoeKit.h>

// Create a sine wave oscillator
KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);

void setup() {
  Serial.begin(115200);
  
  // Initialize KoeKit
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("Failed to initialize KoeKit!");
    while(1);
  }
  
  // Configure oscillator
  osc.setFrequency(440.0f);  // A4 note
  osc.setAmplitude(0.3f);    // 30% volume (safe level)
  
  // Set audio callback - this function generates each audio sample
  KoeKit::setAudioCallback([]() -> float {
    return osc.process();
  });
  
  Serial.println("Playing 440Hz sine wave...");
}

void loop() {
  // Audio generation happens automatically in the background
  // You can do other tasks here
  delay(1000);
  Serial.println("Still playing...");
}
```

**Upload this code and you should hear a pure 440Hz tone!**

## Basic Concepts

### Audio Callback Function

The heart of KoeKit is the audio callback function. This function is called automatically at the sample rate (22,050 times per second) to generate audio:

```cpp
KoeKit::setAudioCallback([]() -> float {
  // This function must return a float between -1.0 and 1.0
  return osc.process();
});
```

**Important:**
- Return values should be between -1.0 and 1.0
- Keep processing lightweight (avoid complex calculations)
- Avoid `Serial.print()` or other slow operations in the callback

### Oscillators

Oscillators generate periodic waveforms:

```cpp
// Create oscillator with specific waveform
KoeKit::Oscillator sineOsc(KoeKit::Wavetables::Basic::SINE);
KoeKit::Oscillator sawOsc(KoeKit::Wavetables::Basic::SAW);

// Configure oscillator
sineOsc.setFrequency(440.0f);    // Set frequency in Hz
sineOsc.setAmplitude(0.5f);      // Set amplitude (0.0 to 1.0)

// Generate samples
float sample = sineOsc.process();
```

### Available Waveforms

```cpp
KoeKit::Wavetables::Basic::SINE      // Pure sine wave
KoeKit::Wavetables::Basic::SAW       // Bright sawtooth
KoeKit::Wavetables::Basic::SQUARE    // Classic square wave
KoeKit::Wavetables::Basic::TRIANGLE  // Soft triangle wave
KoeKit::Wavetables::Basic::SOFT_SAW  // Band-limited sawtooth
KoeKit::Wavetables::Basic::PULSE     // 25% pulse wave
```

### Mixing Multiple Oscillators

```cpp
KoeKit::Oscillator osc1(KoeKit::Wavetables::Basic::SINE);
KoeKit::Oscillator osc2(KoeKit::Wavetables::Basic::SAW);

void setup() {
  KoeKit::begin();
  
  osc1.setFrequency(440.0f);
  osc2.setFrequency(880.0f);  // One octave higher
  
  KoeKit::setAudioCallback([]() -> float {
    float sample1 = osc1.process();
    float sample2 = osc2.process();
    return (sample1 + sample2) * 0.5f;  // Mix and scale
  });
}
```

## Common Patterns

### Musical Notes

```cpp
// Musical note frequencies (in Hz)
const float C4 = 261.63f;
const float D4 = 293.66f;
const float E4 = 329.63f;
const float F4 = 349.23f;
const float G4 = 392.00f;
const float A4 = 440.00f;
const float B4 = 493.88f;
const float C5 = 523.25f;

osc.setFrequency(A4);  // Play A4
```

### Frequency Sweeps

```cpp
float currentFreq = 100.0f;
float targetFreq = 1000.0f;
bool sweepingUp = true;

KoeKit::setAudioCallback([]() -> float {
  // Update frequency every sample
  if (sweepingUp) {
    currentFreq += 2.0f;  // 2Hz per sample
    if (currentFreq >= targetFreq) sweepingUp = false;
  } else {
    currentFreq -= 2.0f;
    if (currentFreq <= 100.0f) sweepingUp = true;
  }
  
  osc.setFrequency(currentFreq);
  return osc.process();
});
```

### User Input Control

```cpp
const int FREQ_POT_PIN = A0;

void loop() {
  // Read potentiometer
  int potValue = analogRead(FREQ_POT_PIN);
  float frequency = map(potValue, 0, 1023, 100, 2000);  // 100Hz to 2kHz
  
  osc.setFrequency(frequency);
  
  delay(50);  // Update 20 times per second
}
```

### Button-Triggered Notes

```cpp
const int BUTTON_PIN = 2;
bool lastButtonState = HIGH;

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);
  
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    // Button pressed - play note
    osc.setAmplitude(0.5f);
  } else if (lastButtonState == LOW && currentButtonState == HIGH) {
    // Button released - stop note
    osc.setAmplitude(0.0f);
  }
  
  lastButtonState = currentButtonState;
  delay(10);
}
```

## Troubleshooting

### No Sound Output

**Check:**
1. **Wiring**: Verify connections between RP2350A and speaker
2. **Pin number**: Ensure correct pin specified in `KoeKit::begin()`
3. **Volume**: Try increasing amplitude (`osc.setAmplitude(0.8f)`)
4. **Board selection**: Verify RP2350A board is selected in Arduino IDE

### Distorted Sound

**Solutions:**
1. **Lower amplitude**: Try `osc.setAmplitude(0.2f)`
2. **Add filtering**: Use RC low-pass filter on output
3. **Check power supply**: Ensure stable power

### Compilation Errors

**Common fixes:**
1. **Update arduino-pico**: Ensure version 4.0.0 or later
2. **Include path**: Verify KoeKit is in correct libraries folder
3. **Restart IDE**: Close and reopen Arduino IDE

### Performance Issues

**If audio is choppy:**
1. **Lower sample rate**: Try `KoeKit::begin(11025, 1)`
2. **Simplify callback**: Remove complex calculations from audio callback
3. **Avoid Serial.print()**: Don't use Serial in audio callback

### Memory Issues

**If compilation fails with memory errors:**
1. **Reduce wavetable usage**: Only include needed waveforms
2. **Use simpler data types**: Consider using smaller wavetables
3. **Optimize code**: Remove unused variables and functions

### Getting Help

1. **Check examples**: Look at included example sketches
2. **API Reference**: Consult the complete API documentation
3. **GitHub Issues**: Report bugs or ask questions on GitHub
4. **Serial Monitor**: Use Serial.println() for debugging (outside audio callback)

### Debug Template

```cpp
#include <KoeKit.h>

void setup() {
  Serial.begin(115200);
  delay(2000);  // Wait for serial connection
  
  Serial.println("KoeKit Debug Test");
  Serial.print("Free RAM: ");
  Serial.println(rp2040.getFreeHeap());
  
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("❌ KoeKit initialization failed!");
    return;
  }
  
  Serial.println("✅ KoeKit initialized successfully");
  Serial.print("Sample rate: ");
  Serial.println(KoeKit::getSampleRate());
  
  // Test basic oscillator
  KoeKit::Oscillator testOsc(KoeKit::Wavetables::Basic::SINE);
  testOsc.setFrequency(440.0f);
  testOsc.setAmplitude(0.3f);
  
  KoeKit::setAudioCallback([]() -> float {
    return testOsc.process();
  });
  
  Serial.println("✅ Test tone should be playing on pin 1");
}

void loop() {
  delay(1000);
  Serial.println("Audio system running...");
}
```

## Next Steps

Once you have basic sound working:

1. **Try different waveforms**: Experiment with saw, square, triangle waves
2. **Add filters**: Learn about KoeKit's filter modules
3. **Use envelopes**: Add ADSR envelopes for more musical sounds
4. **Create custom waveforms**: Generate your own wavetables
5. **Build a synthesizer**: Combine oscillators, filters, and envelopes

Check out the `examples/` folder for more advanced usage patterns!
# Hardware Setup

This guide covers the hardware connections and circuit designs for optimal KoeKit audio output.

## Supported Hardware

### Primary Target
- **Raspberry Pi Pico 2** (RP2350A)
- Any RP2350A-based development board

::: warning Compatibility
KoeKit is currently optimized specifically for RP2350A. RP2040 support may be added in future versions.
:::

## Audio Output Circuits

### 1. Minimal Setup (Testing)

For quick testing and prototyping:

```
RP2350A Pin 1 ─── [1kΩ] ─── [Speaker 8Ω] ─── GND
```

**Pros:**
- Simplest possible connection
- Good for initial testing

**Cons:**
- PWM artifacts audible
- Limited volume
- Square wave artifacts

### 2. RC Low-Pass Filter (Recommended)

For better audio quality:

```
RP2350A Pin 1 ─── [1kΩ] ──┬─── [Speaker 8Ω] ─── GND
                           │
                          [100nF]
                           │
                          GND
```

**Component Values:**
- R = 1kΩ
- C = 100nF
- Cutoff frequency ≈ 1.6kHz

**Pros:**
- Removes most PWM artifacts
- Cleaner audio output
- Still very simple

**Cons:**
- Slight high-frequency roll-off

### 3. Professional Setup (Best Quality)

For the highest audio quality:

```
RP2350A Pin 1 ─── [RC Filter] ─── [Audio Amplifier] ─── [Speaker]
```

**RC Filter Stage:**
```
Pin 1 ─── [1kΩ] ──┬─── [470Ω] ──┬─── To Amplifier
                  │              │
                 [100nF]        [220nF]
                  │              │
                 GND            GND
```

**Recommended Amplifiers:**
- LM386 (simple, low power)
- PAM8403 (stereo, higher quality)
- TDA2030 (higher power)

## Pin Configuration

### Default Pin Assignment

| Function | Pin | Notes |
|----------|-----|-------|
| Audio Output | GPIO 1 | PWM output, configurable |
| Ground | GND | Common ground |

### Alternative Pins

KoeKit can use any GPIO pin capable of PWM:

```cpp
// Use different output pin
KoeKit::begin(22050, 2);  // Use GPIO 2 instead
```

**Available PWM pins on RP2350A:**
- GPIO 0-15 (all support PWM)

## Power Considerations

### USB Power
- **Typical current draw**: 50-80mA
- **USB power**: Sufficient for most applications
- **Voltage**: 3.3V logic, 5V USB input

### External Power
For high-volume applications or when driving multiple speakers:

```
External 5V ─── [Voltage Regulator 3.3V] ─── RP2350A VCC
     │                                            │
    GND ──────────────────────────────────────── GND
```

## Audio Quality Optimization

### 1. PWM Frequency

KoeKit automatically sets PWM frequency to 100kHz for optimal audio quality:

```cpp
// This is handled automatically
analogWriteFreq(100000);  // 100kHz PWM
```

### 2. Grounding

Proper grounding is crucial for clean audio:

- Use short, direct ground connections
- Star grounding topology when possible
- Avoid ground loops

### 3. Power Supply Filtering

Add capacitors near the RP2350A:

```
VCC ─── [10µF] ──┬─── [100nF] ─── RP2350A VCC
                 │
                GND
```

## Stereo Output (Future)

KoeKit will support stereo output using two PWM pins:

```cpp
// Future feature
KoeKit::beginStereo(22050, 1, 2);  // Left=Pin1, Right=Pin2
```

## Testing Your Setup

### 1. Continuity Test
Use a multimeter to verify all connections before powering on.

### 2. Audio Test
Upload the SimpleTone example and verify you hear a 440Hz tone.

### 3. Quality Check
Listen for:
- ✅ Clear, pure tone
- ❌ Buzzing or artifacts
- ❌ Distortion at higher volumes

## Common Issues

### No Sound Output

**Check:**
1. Wiring connections
2. Speaker polarity (if applicable)
3. Power supply voltage
4. Code compilation errors

### Distorted Sound

**Solutions:**
1. Lower amplitude in code
2. Add/improve RC filtering
3. Check power supply stability
4. Verify speaker impedance (8Ω minimum)

### Artifacts/Noise

**Solutions:**
1. Add better filtering
2. Improve grounding
3. Check for electromagnetic interference
4. Use shielded cables for longer connections

## Advanced Setups

### Multiple Output Channels

For complex projects requiring multiple audio outputs:

```cpp
// Multiple independent outputs
KoeKit::PWMAudioOutput output1, output2;
output1.begin(1, 22050);  // Pin 1
output2.begin(2, 22050);  // Pin 2
```

### Mixing External Audio

Combine KoeKit output with external audio sources:

```
KoeKit Output ─── [10kΩ] ──┬─── [Audio Mixer] ─── [Amplifier]
External Audio ── [10kΩ] ──┘
```

## Safety Guidelines

::: danger High Volume Warning
- Always start with low amplitudes (0.1-0.3)
- Use hearing protection when testing
- Be aware of speaker power ratings
:::

::: warning Circuit Safety
- Double-check connections before powering on
- Use current-limiting resistors
- Ensure proper ventilation for amplifiers
- Follow local electrical codes for permanent installations
:::

## Component Shopping List

### Basic Setup
- 1kΩ resistor (1/4W)
- 100nF ceramic capacitor
- 8Ω speaker (small)
- Breadboard and jumper wires

### Professional Setup
- Additional 470Ω resistor
- 220nF capacitor
- Audio amplifier module
- Larger speaker (appropriate for amplifier)
- Enclosure and mounting hardware

Most components are available from electronics suppliers like Adafruit, SparkFun, Digi-Key, or Mouser.
# Quick Start

Get KoeKit running in under 5 minutes with this condensed guide.

## Prerequisites

- RP2350A board (Raspberry Pi Pico 2)
- Arduino IDE with arduino-pico installed
- Speaker or headphones

## 1. Install KoeKit

Download and extract KoeKit to your Arduino libraries folder:

```
~/Documents/Arduino/libraries/KoeKit/
```

## 2. Basic Circuit

Connect a speaker to your RP2350A:

```
RP2350A Pin 1 → [1kΩ resistor] → [Speaker] → GND
```

## 3. First Code

Create a new Arduino sketch:

```cpp
#include <KoeKit.h>

KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);

void setup() {
  Serial.begin(115200);
  
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("KoeKit failed to start!");
    while(1);
  }
  
  osc.setFrequency(440.0f);
  osc.setAmplitude(0.3f);
  
  KoeKit::setAudioCallback([]() -> float {
    return osc.process();
  });
  
  Serial.println("Playing 440Hz tone...");
}

void loop() {
  delay(1000);
}
```

## 4. Upload and Test

1. Select **Tools → Board → Raspberry Pi Pico 2**
2. Select your serial port
3. Upload the sketch
4. You should hear a 440Hz sine wave!

## Next Steps

- [Try different waveforms](/examples/basic#waveform-test)
- [Add filters and envelopes](/examples/filters)
- [Create custom wavetables](/examples/custom-wavetables)
- [Build a complete synthesizer](/examples/synthesizers)

::: warning Volume Warning
Start with low amplitude values (0.1-0.3) to protect your hearing and equipment.
:::

::: tip Troubleshooting
If you don't hear sound:
- Check wiring connections
- Verify correct board selection
- Try increasing amplitude to 0.5f
- Check serial monitor for error messages
:::
/**
 * @file CustomWavetable.ino
 * @brief Custom wavetable creation example
 * 
 * This example demonstrates how to create custom wavetables:
 * - Generate wavetables from mathematical formulas
 * - Use custom sample arrays
 * - Mix different harmonic content
 * 
 * Creates several custom waveforms and cycles through them.
 * 
 * Hardware:
 * - RP2350A board
 * - Speaker connected to pin 1
 */

#include <KoeKit.h>

// Create custom wavetables at compile time
constexpr auto customWave1 = KoeKit::makeWavetable<1024>([](size_t i) -> float {
  // Harmonic-rich wave: fundamental + 3rd + 5th harmonics
  float phase = 2.0f * M_PI * static_cast<float>(i) / 1024.0f;
  return 0.6f * sin(phase) + 0.3f * sin(3.0f * phase) + 0.1f * sin(5.0f * phase);
});

constexpr auto customWave2 = KoeKit::makeWavetable<1024>([](size_t i) -> float {
  // Ring modulated sine wave
  float phase = 2.0f * M_PI * static_cast<float>(i) / 1024.0f;
  return sin(phase) * sin(7.0f * phase) * 0.5f;
});

constexpr auto customWave3 = KoeKit::makeWavetable<1024>([](size_t i) -> float {
  // Stepped wave (quantized)
  float phase = 2.0f * M_PI * static_cast<float>(i) / 1024.0f;
  float sine = sin(phase);
  return floor(sine * 4.0f) / 4.0f; // 4-level quantization
});

constexpr auto customWave4 = KoeKit::makeWavetable<1024>([](size_t i) -> float {
  // Exponential decay sine burst
  float t = static_cast<float>(i) / 1024.0f;
  float phase = 2.0f * M_PI * t;
  return sin(8.0f * phase) * exp(-t * 3.0f);
});

// Create custom wavetable from sample array
constexpr auto makeFuzzWave() {
  std::array<float, 1024> samples{};
  
  // Create a "fuzz" distorted sine wave
  for (size_t i = 0; i < 1024; ++i) {
    float phase = 2.0f * M_PI * static_cast<float>(i) / 1024.0f;
    float sine = sin(phase);
    
    // Soft clipping distortion
    if (sine > 0.7f) sine = 0.7f + (sine - 0.7f) * 0.2f;
    if (sine < -0.7f) sine = -0.7f + (sine + 0.7f) * 0.2f;
    
    samples[i] = sine;
  }
  
  return KoeKit::makeWavetable(samples);
}

constexpr auto customWave5 = makeFuzzWave();

// Oscillator
KoeKit::Oscillator osc(customWave1);

// Control variables
int currentWave = 0;
unsigned long lastSwitchTime = 0;
const unsigned long SWITCH_INTERVAL = 3000; // 3 seconds per wave

const char* waveNames[] = {
  "Harmonic Rich",
  "Ring Modulated", 
  "Stepped/Quantized",
  "Exponential Burst",
  "Fuzz Distorted"
};

void setup() {
  Serial.begin(115200);
  Serial.println("KoeKit Custom Wavetable Example");
  
  // Initialize KoeKit
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("Failed to initialize KoeKit!");
    while (1);
  }
  
  Serial.println("KoeKit initialized");
  
  // Configure oscillator
  osc.setFrequency(220.0f);  // A3
  osc.setAmplitude(0.4f);
  
  // Set audio callback
  KoeKit::setAudioCallback([]() -> float {
    return osc.process();
  });
  
  Serial.println("Playing custom wavetables...");
  Serial.print("Starting with: ");
  Serial.println(waveNames[0]);
}

void loop() {
  // Switch waveforms every 3 seconds
  if (millis() - lastSwitchTime > SWITCH_INTERVAL) {
    currentWave = (currentWave + 1) % 5;
    switchToWave(currentWave);
    lastSwitchTime = millis();
  }
  
  delay(100);
}

void switchToWave(int waveIndex) {
  switch (waveIndex) {
    case 0:
      osc.setWavetable(customWave1);
      break;
    case 1:
      osc.setWavetable(customWave2);
      break;
    case 2:
      osc.setWavetable(customWave3);
      break;
    case 3:
      osc.setWavetable(customWave4);
      break;
    case 4:
      osc.setWavetable(customWave5);
      break;
  }
  
  Serial.print("Now playing: ");
  Serial.println(waveNames[waveIndex]);
}
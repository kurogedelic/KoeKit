/**
 * @file FrequencySweep.ino
 * @brief Frequency sweep demonstration using KoeKit
 * 
 * This example demonstrates:
 * - Smooth frequency transitions
 * - Real-time parameter control
 * - Multiple oscillator control
 * 
 * Creates a frequency sweep from 100Hz to 1000Hz and back,
 * cycling through different waveforms.
 * 
 * Hardware:
 * - RP2350A board
 * - Speaker connected to pin 1  
 * - Optional potentiometer on A0 for speed control
 */

#include <KoeKit.h>

// Oscillator
KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);

// Sweep parameters
float currentFreq = 100.0f;
float targetFreq = 1000.0f;
bool sweepingUp = true;
const float SWEEP_RATE = 2.0f; // Hz per sample at 22kHz

// Waveform cycling
int currentWaveform = 0;
unsigned long lastWaveformChange = 0;
const unsigned long WAVEFORM_CHANGE_INTERVAL = 4000; // 4 seconds

// Speed control (optional)
const int SPEED_POT_PIN = A0;

void setup() {
  Serial.begin(115200);
  Serial.println("KoeKit Frequency Sweep Example");
  
  // Initialize KoeKit
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("Failed to initialize KoeKit!");
    while (1);
  }
  
  Serial.println("KoeKit initialized");
  
  // Configure oscillator
  osc.setFrequency(currentFreq);
  osc.setAmplitude(0.3f);
  
  // Set audio callback
  KoeKit::setAudioCallback([]() -> float {
    updateSweep();
    return osc.process();
  });
  
  Serial.println("Starting frequency sweep...");
  Serial.println("100Hz -> 1000Hz -> 100Hz...");
  
  if (analogRead(SPEED_POT_PIN) != 0) {
    Serial.println("Potentiometer on A0 controls sweep speed");
  }
}

void loop() {
  // Change waveform periodically
  if (millis() - lastWaveformChange > WAVEFORM_CHANGE_INTERVAL) {
    currentWaveform = (currentWaveform + 1) % 6;
    auto waveform = static_cast<KoeKit::Wavetables::Basic::Waveform>(currentWaveform);
    osc.setWavetable(KoeKit::Wavetables::Basic::getWavetable(waveform));
    
    const char* names[] = {"Sine", "Saw", "Square", "Triangle", "Soft Saw", "Pulse"};
    Serial.print("Waveform: ");
    Serial.println(names[currentWaveform]);
    
    lastWaveformChange = millis();
  }
  
  // Print current frequency periodically
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    Serial.print("Frequency: ");
    Serial.print(currentFreq, 1);
    Serial.println(" Hz");
    lastPrint = millis();
  }
  
  delay(100);
}

void updateSweep() {
  // Get sweep speed from potentiometer (if connected)
  float speedMultiplier = 1.0f;
  int potValue = analogRead(SPEED_POT_PIN);
  if (potValue > 10) { // Pot is connected
    speedMultiplier = map(potValue, 0, 1023, 1, 50) / 10.0f; // 0.1x to 5.0x speed
  }
  
  float sweepStep = SWEEP_RATE * speedMultiplier;
  
  // Update frequency
  if (sweepingUp) {
    currentFreq += sweepStep;
    if (currentFreq >= targetFreq) {
      currentFreq = targetFreq;
      sweepingUp = false;
      targetFreq = 100.0f;
    }
  } else {
    currentFreq -= sweepStep;
    if (currentFreq <= targetFreq) {
      currentFreq = targetFreq;
      sweepingUp = true;
      targetFreq = 1000.0f;
    }
  }
  
  // Apply frequency to oscillator
  osc.setFrequency(currentFreq);
}
/**
 * @file WaveformTest.ino
 * @brief Test all basic waveforms in KoeKit
 * 
 * This example cycles through all available basic waveforms:
 * - Sine wave
 * - Sawtooth wave  
 * - Square wave
 * - Triangle wave
 * - Soft sawtooth
 * - Pulse wave
 * 
 * Each waveform plays for 2 seconds before switching to the next.
 * 
 * Hardware:
 * - RP2350A board
 * - Speaker connected to pin 1
 * - Optional button on pin 2 to manually advance waveforms
 */

#include <KoeKit.h>

// Create oscillator
KoeKit::Oscillator osc(KoeKit::Wavetables::Basic::SINE);

// Waveform names for serial output
const char* waveformNames[] = {
  "Sine",
  "Sawtooth", 
  "Square",
  "Triangle",
  "Soft Sawtooth",
  "Pulse"
};

// Current waveform index
int currentWaveform = 0;
unsigned long lastSwitchTime = 0;
const unsigned long SWITCH_INTERVAL = 2000; // 2 seconds per waveform

// Button for manual control
const int BUTTON_PIN = 2;
bool lastButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  Serial.println("KoeKit Waveform Test");
  
  // Setup button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize KoeKit
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("Failed to initialize KoeKit!");
    while (1);
  }
  
  Serial.println("KoeKit initialized");
  
  // Configure oscillator
  osc.setFrequency(220.0f);  // A3 note
  osc.setAmplitude(0.4f);    // Safe volume
  
  // Set initial waveform
  setWaveform(0);
  
  // Set audio callback
  KoeKit::setAudioCallback([]() -> float {
    return osc.process();
  });
  
  Serial.println("Starting waveform test...");
  Serial.println("Button on pin 2 to manually advance");
}

void loop() {
  // Check for button press (manual advance)
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    // Button pressed
    advanceWaveform();
    delay(200); // Debounce
  }
  lastButtonState = currentButtonState;
  
  // Auto-advance every 2 seconds
  if (millis() - lastSwitchTime > SWITCH_INTERVAL) {
    advanceWaveform();
  }
  
  delay(50);
}

void advanceWaveform() {
  currentWaveform = (currentWaveform + 1) % 6;
  setWaveform(currentWaveform);
  lastSwitchTime = millis();
}

void setWaveform(int index) {
  // Cast to waveform enum
  auto waveform = static_cast<KoeKit::Wavetables::Basic::Waveform>(index);
  
  // Set the new wavetable
  osc.setWavetable(KoeKit::Wavetables::Basic::getWavetable(waveform));
  
  Serial.print("Now playing: ");
  Serial.println(waveformNames[index]);
}
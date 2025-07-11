/**
 * @file SimpleTone.ino
 * @brief Basic KoeKit example - Simple sine wave tone
 * 
 * This example demonstrates the basic usage of KoeKit:
 * - Initialize the audio system
 * - Create a sine wave oscillator
 * - Generate a 440Hz tone
 * 
 * Hardware:
 * - RP2350A board (Raspberry Pi Pico 2 or compatible)
 * - Speaker or headphones connected to pin 1 (default PWM output)
 * - Optional: Low-pass filter (RC filter) for better audio quality
 * 
 * Circuit:
 * Pin 1 -> [1kΩ resistor] -> [Speaker] -> GND
 *       -> [100nF capacitor] -> GND (optional filter)
 */

#include <KoeKit.h>

// Create a sine wave oscillator
KoeKit::Oscillator sineOsc(KoeKit::Wavetables::Basic::SINE);

void setup() {
  Serial.begin(115200);
  Serial.println("KoeKit SimpleTone Example");
  
  // Initialize KoeKit audio system
  // Sample rate: 22050 Hz, Output pin: 1
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("Failed to initialize KoeKit!");
    while (1);
  }
  
  Serial.println("KoeKit initialized successfully");
  
  // Configure oscillator
  sineOsc.setFrequency(440.0f);  // A4 note (440 Hz)
  sineOsc.setAmplitude(0.3f);    // 30% amplitude (safe volume)
  
  // Set audio callback
  KoeKit::setAudioCallback([]() -> float {
    return sineOsc.process();
  });
  
  Serial.println("Playing 440Hz sine wave...");
  Serial.println("Audio output on pin 1");
}

void loop() {
  // Main loop can do other tasks
  // Audio generation happens in the background via interrupts
  
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 1000) {
    Serial.println("Audio running...");
    lastTime = millis();
  }
  
  delay(100);
}
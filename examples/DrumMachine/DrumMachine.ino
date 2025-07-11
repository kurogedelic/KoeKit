/**
 * @file DrumMachine.ino
 * @brief Simple drum machine using KoeKit synthesis
 * 
 * This example demonstrates:
 * - Percussive synthesis techniques
 * - Multiple sound generators (kick, snare, hi-hat)
 * - Sequencer with programmable patterns
 * - Real-time tempo control
 * 
 * Sounds generated:
 * - Kick: Sine wave with pitch envelope
 * - Snare: Filtered noise with quick decay
 * - Hi-hat: High-frequency filtered noise
 * 
 * Hardware:
 * - RP2350A board
 * - Speaker connected to pin 1
 * - Optional buttons on pins 2-4 for manual triggers
 * - Optional potentiometer on A0 for tempo control
 */

#include <KoeKit.h>

// Drum sound classes
class KickDrum {
private:
  KoeKit::Oscillator osc;
  KoeKit::Envelope::AR pitchEnv;
  KoeKit::Envelope::AR ampEnv;
  
public:
  KickDrum() : osc(KoeKit::Wavetables::Basic::SINE) {
    osc.setAmplitude(1.0f);
    
    // Fast pitch envelope for "thump"
    pitchEnv.setAR(0.001f, 0.1f);
    
    // Amplitude envelope
    ampEnv.setAR(0.001f, 0.3f);
  }
  
  void trigger() {
    pitchEnv.trigger();
    ampEnv.trigger();
  }
  
  float process() {
    float pitchMod = pitchEnv.process();
    
    // Start at ~80Hz, drop to ~40Hz
    float frequency = 40.0f + pitchMod * 40.0f;
    osc.setFrequency(frequency);
    
    float sample = osc.process();
    return ampEnv.process(sample);
  }
};

class SnareDrum {
private:
  KoeKit::NoiseGenerator noise;
  KoeKit::Oscillator tone;
  KoeKit::Envelope::AR env;
  KoeKit::Filter::Biquad filter;
  
public:
  SnareDrum() : tone(KoeKit::Wavetables::Basic::TRIANGLE) {
    noise.setAmplitude(0.8f);
    tone.setFrequency(200.0f);
    tone.setAmplitude(0.3f);
    
    env.setAR(0.001f, 0.12f);
    
    // Band-pass filter for snare character
    filter.setBandPass(1000.0f, 800.0f);
  }
  
  void trigger() {
    env.trigger();
  }
  
  float process() {
    float noiseSample = noise.process();
    float toneSample = tone.process();
    
    // Mix noise and tone
    float mixed = noiseSample * 0.7f + toneSample * 0.3f;
    
    // Apply filter and envelope
    float filtered = filter.process(mixed);
    return env.process(filtered);
  }
};

class HiHat {
private:
  KoeKit::NoiseGenerator noise;
  KoeKit::Envelope::AR env;
  KoeKit::Filter::Biquad filter;
  bool isOpen;
  
public:
  HiHat() {
    noise.setAmplitude(0.6f);
    env.setAR(0.001f, 0.08f);  // Quick decay
    
    // High-pass filter for crisp hi-hat sound
    filter.setHighPass(8000.0f);
    isOpen = false;
  }
  
  void triggerClosed() {
    isOpen = false;
    env.setAR(0.001f, 0.08f);  // Short decay
    env.trigger();
  }
  
  void triggerOpen() {
    isOpen = true;
    env.setAR(0.001f, 0.25f);  // Longer decay
    env.trigger();
  }
  
  float process() {
    float sample = noise.process();
    sample = filter.process(sample);
    return env.process(sample);
  }
};

// Drum machine class
class DrumMachine {
private:
  KickDrum kick;
  SnareDrum snare;
  HiHat hihat;
  
  // Sequencer variables
  int currentStep = 0;
  unsigned long lastStepTime = 0;
  int bpm = 120;
  
  // Pattern storage (16 steps)
  bool kickPattern[16] = {1,0,0,0, 1,0,1,0, 1,0,0,0, 1,0,0,0};
  bool snarePattern[16] = {0,0,0,0, 1,0,0,0, 0,0,0,0, 1,0,0,0};
  bool hihatPattern[16] = {1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0};
  
public:
  DrumMachine() {}
  
  void setBPM(int newBPM) {
    bpm = constrain(newBPM, 60, 200);
  }
  
  int getBPM() const {
    return bpm;
  }
  
  void update() {
    unsigned long stepInterval = 60000 / (bpm * 4);  // 16th notes
    
    if (millis() - lastStepTime >= stepInterval) {
      playStep(currentStep);
      currentStep = (currentStep + 1) % 16;
      lastStepTime = millis();
    }
  }
  
  void playStep(int step) {
    if (kickPattern[step]) {
      kick.trigger();
    }
    if (snarePattern[step]) {
      snare.trigger();
    }
    if (hihatPattern[step]) {
      hihat.triggerClosed();
    }
  }
  
  // Manual triggers
  void triggerKick() { kick.trigger(); }
  void triggerSnare() { snare.trigger(); }
  void triggerHiHat() { hihat.triggerClosed(); }
  void triggerOpenHiHat() { hihat.triggerOpen(); }
  
  float process() {
    float output = 0.0f;
    
    output += kick.process() * 0.8f;     // Kick is loudest
    output += snare.process() * 0.6f;    // Snare medium
    output += hihat.process() * 0.4f;    // Hi-hat quietest
    
    return output;
  }
  
  int getCurrentStep() const {
    return currentStep;
  }
};

// Global objects
DrumMachine drumMachine;

// Control pins
const int KICK_BUTTON_PIN = 2;
const int SNARE_BUTTON_PIN = 3;
const int HIHAT_BUTTON_PIN = 4;
const int TEMPO_POT_PIN = A0;

// Button state tracking
bool lastKickButton = HIGH;
bool lastSnareButton = HIGH;
bool lastHihatButton = HIGH;

void setup() {
  Serial.begin(115200);
  Serial.println("KoeKit Drum Machine");
  
  // Setup control pins
  pinMode(KICK_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SNARE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(HIHAT_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize KoeKit
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("Failed to initialize KoeKit!");
    while (1);
  }
  
  Serial.println("KoeKit initialized successfully");
  
  // Set audio callback
  KoeKit::setAudioCallback([]() -> float {
    return drumMachine.process();
  });
  
  Serial.println("Drum machine ready!");
  Serial.println("Automatic pattern playing...");
  Serial.println("Manual triggers:");
  Serial.println("  Pin 2: Kick");
  Serial.println("  Pin 3: Snare");
  Serial.println("  Pin 4: Hi-hat");
  Serial.println("  A0: Tempo control");
  
  Serial.print("Initial BPM: ");
  Serial.println(drumMachine.getBPM());
}

void loop() {
  // Update sequencer
  drumMachine.update();
  
  // Handle manual triggers
  handleManualTriggers();
  
  // Update tempo from potentiometer
  updateTempoFromPot();
  
  // Print sequencer status
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 500) {
    printSequencerStatus();
    lastStatus = millis();
  }
  
  delay(10);
}

void handleManualTriggers() {
  // Kick trigger
  bool currentKickButton = digitalRead(KICK_BUTTON_PIN);
  if (lastKickButton == HIGH && currentKickButton == LOW) {
    drumMachine.triggerKick();
    Serial.println("Manual Kick");
  }
  lastKickButton = currentKickButton;
  
  // Snare trigger
  bool currentSnareButton = digitalRead(SNARE_BUTTON_PIN);
  if (lastSnareButton == HIGH && currentSnareButton == LOW) {
    drumMachine.triggerSnare();
    Serial.println("Manual Snare");
  }
  lastSnareButton = currentSnareButton;
  
  // Hi-hat trigger
  bool currentHihatButton = digitalRead(HIHAT_BUTTON_PIN);
  if (lastHihatButton == HIGH && currentHihatButton == LOW) {
    drumMachine.triggerHiHat();
    Serial.println("Manual Hi-hat");
  }
  lastHihatButton = currentHihatButton;
}

void updateTempoFromPot() {
  static unsigned long lastTempoUpdate = 0;
  
  // Update tempo every 200ms to avoid jitter
  if (millis() - lastTempoUpdate > 200) {
    int potValue = analogRead(TEMPO_POT_PIN);
    
    if (potValue > 10) {  // Only if pot is connected
      int newBPM = map(potValue, 0, 1023, 80, 160);
      
      if (abs(newBPM - drumMachine.getBPM()) > 2) {  // Only update if significant change
        drumMachine.setBPM(newBPM);
        Serial.print("BPM: ");
        Serial.println(newBPM);
      }
    }
    
    lastTempoUpdate = millis();
  }
}

void printSequencerStatus() {
  int currentStep = drumMachine.getCurrentStep();
  
  Serial.print("Step: ");
  if (currentStep < 10) Serial.print(" ");
  Serial.print(currentStep + 1);
  Serial.print("/16  BPM: ");
  Serial.print(drumMachine.getBPM());
  
  // Visual step indicator
  Serial.print("  [");
  for (int i = 0; i < 16; i++) {
    if (i == currentStep) {
      Serial.print("*");
    } else if (i % 4 == 0) {
      Serial.print("|");
    } else {
      Serial.print("-");
    }
  }
  Serial.println("]");
}
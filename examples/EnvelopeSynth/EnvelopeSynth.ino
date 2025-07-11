/**
 * @file EnvelopeSynth.ino
 * @brief Complete synthesizer with ADSR envelope and filter envelope
 * 
 * This example demonstrates:
 * - ADSR envelope generator
 * - Filter envelope for dynamic timbre changes
 * - LFO modulation
 * - Multiple oscillators with detuning
 * - Real-time parameter control
 * 
 * Hardware:
 * - RP2350A board
 * - Speaker connected to pin 1
 * - Button on pin 2 for note trigger
 * - Optional potentiometers:
 *   - A0: Filter cutoff
 *   - A1: Filter envelope amount
 *   - A2: LFO speed
 */

#include <KoeKit.h>

// Audio components
KoeKit::Oscillator osc1(KoeKit::Wavetables::Basic::SAW);
KoeKit::Oscillator osc2(KoeKit::Wavetables::Basic::SQUARE);
KoeKit::Filter::StateVariable filter;
KoeKit::Envelope::ADSR ampEnvelope;
KoeKit::Envelope::ADSR filterEnvelope;
KoeKit::Envelope::LFO vibrato;

// Control variables
const int TRIGGER_PIN = 2;
const int FILTER_CUTOFF_PIN = A0;
const int FILTER_ENV_PIN = A1;
const int LFO_SPEED_PIN = A2;

bool lastButtonState = HIGH;
float baseFrequency = 220.0f;  // A3 note
float baseCutoff = 800.0f;
float filterEnvAmount = 1500.0f;

// Envelope parameters
struct EnvelopeParams {
  float attack = 0.02f;
  float decay = 0.3f;
  float sustain = 0.6f;
  float release = 0.8f;
};

EnvelopeParams ampParams = {0.02f, 0.3f, 0.6f, 0.8f};
EnvelopeParams filterParams = {0.01f, 0.5f, 0.3f, 1.0f};

void setup() {
  Serial.begin(115200);
  Serial.println("KoeKit Envelope Synthesizer");
  
  // Setup pins
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  
  // Initialize KoeKit
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("Failed to initialize KoeKit!");
    while (1);
  }
  
  Serial.println("KoeKit initialized successfully");
  
  // Configure oscillators
  osc1.setFrequency(baseFrequency);
  osc2.setFrequency(baseFrequency * 1.007f);  // Slight detune for richness
  osc1.setAmplitude(0.6f);
  osc2.setAmplitude(0.4f);
  
  // Configure filter
  filter.setParams(baseCutoff, 2.0f);  // Moderate resonance
  
  // Configure envelopes
  ampEnvelope.setADSR(ampParams.attack, ampParams.decay, 
                      ampParams.sustain, ampParams.release);
  
  filterEnvelope.setADSR(filterParams.attack, filterParams.decay,
                         filterParams.sustain, filterParams.release);
  
  // Configure LFO for vibrato
  vibrato.setFrequency(4.0f);
  vibrato.setAmplitude(0.02f);  // Subtle vibrato
  vibrato.setWaveform(KoeKit::Envelope::LFO::Waveform::SINE);
  
  // Set audio callback
  KoeKit::setAudioCallback([]() -> float {
    return processSynthesis();
  });
  
  Serial.println("Synthesizer ready!");
  Serial.println("Press button on pin 2 to trigger notes");
  Serial.println("Use potentiometers on A0-A2 for real-time control");
}

void loop() {
  // Handle note triggering
  handleNoteControl();
  
  // Update parameters from potentiometers
  updateParametersFromPots();
  
  // Print status periodically
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 2000) {
    printStatus();
    lastStatus = millis();
  }
  
  delay(10);
}

float processSynthesis() {
  // Get LFO modulation
  float vibratoAmount = vibrato.process();
  
  // Apply vibrato to oscillator frequencies
  float modFreq1 = baseFrequency * (1.0f + vibratoAmount);
  float modFreq2 = baseFrequency * 1.007f * (1.0f + vibratoAmount);
  
  osc1.setFrequency(modFreq1);
  osc2.setFrequency(modFreq2);
  
  // Mix oscillators
  float oscillatorMix = osc1.process() + osc2.process();
  
  // Get filter envelope value
  float filterEnvValue = filterEnvelope.process();
  
  // Calculate dynamic filter cutoff
  float dynamicCutoff = baseCutoff + (filterEnvValue * filterEnvAmount);
  filter.setParams(dynamicCutoff, 2.0f);
  
  // Apply filter
  filter.process(oscillatorMix);
  float filteredSample = filter.getLowPass();
  
  // Apply amplitude envelope
  return ampEnvelope.process(filteredSample);
}

void handleNoteControl() {
  bool currentButtonState = digitalRead(TRIGGER_PIN);
  
  // Note on (button pressed)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.println("Note ON");
    ampEnvelope.noteOn();
    filterEnvelope.noteOn();
  }
  // Note off (button released)
  else if (lastButtonState == LOW && currentButtonState == HIGH) {
    Serial.println("Note OFF");
    ampEnvelope.noteOff();
    filterEnvelope.noteOff();
  }
  
  lastButtonState = currentButtonState;
}

void updateParametersFromPots() {
  static unsigned long lastUpdate = 0;
  
  // Update parameters every 100ms to avoid jitter
  if (millis() - lastUpdate > 100) {
    
    // Filter cutoff control (A0)
    int cutoffPot = analogRead(FILTER_CUTOFF_PIN);
    if (cutoffPot > 10) {  // Only if pot is connected
      baseCutoff = map(cutoffPot, 0, 1023, 200, 4000);
    }
    
    // Filter envelope amount control (A1)
    int envAmountPot = analogRead(FILTER_ENV_PIN);
    if (envAmountPot > 10) {
      filterEnvAmount = map(envAmountPot, 0, 1023, 0, 3000);
    }
    
    // LFO speed control (A2)
    int lfoSpeedPot = analogRead(LFO_SPEED_PIN);
    if (lfoSpeedPot > 10) {
      float lfoFreq = map(lfoSpeedPot, 0, 1023, 10, 80) / 10.0f;  // 1.0 to 8.0 Hz
      vibrato.setFrequency(lfoFreq);
    }
    
    lastUpdate = millis();
  }
}

void printStatus() {
  Serial.print("Base Cutoff: ");
  Serial.print(baseCutoff, 0);
  Serial.print("Hz, Filter Env: ");
  Serial.print(filterEnvAmount, 0);
  Serial.print("Hz, LFO: ");
  Serial.print(vibrato.getFrequency(), 1);
  Serial.print("Hz");
  
  if (ampEnvelope.isActive()) {
    Serial.print(" [PLAYING]");
  }
  
  Serial.println();
}
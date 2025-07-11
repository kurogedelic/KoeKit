/**
 * @file FilterSweep.ino
 * @brief Demonstrates filter sweep with resonance using KoeKit
 * 
 * This example shows:
 * - State Variable Filter with high resonance
 * - Automatic filter frequency sweeping
 * - Multiple filter output modes
 * - Real-time parameter control
 * 
 * Hardware:
 * - RP2350A board
 * - Speaker connected to pin 1
 * - Optional button on pin 2 to change filter mode
 * - Optional potentiometer on A0 for resonance control
 */

#include <KoeKit.h>

// Audio components
KoeKit::Oscillator sawOsc(KoeKit::Wavetables::Basic::SAW);
KoeKit::Filter::StateVariable filter;

// Control variables
float cutoffFreq = 200.0f;
bool sweepingUp = true;
const float SWEEP_SPEED = 8.0f;  // Hz per sample
const float MIN_CUTOFF = 200.0f;
const float MAX_CUTOFF = 3000.0f;

// Filter mode cycling
int filterMode = 0;  // 0=LP, 1=HP, 2=BP, 3=Notch
const int MODE_BUTTON_PIN = 2;
bool lastButtonState = HIGH;
unsigned long lastModeChange = 0;

// Resonance control
const int RESONANCE_POT_PIN = A0;

const char* filterModeNames[] = {
  "Low-Pass",
  "High-Pass", 
  "Band-Pass",
  "Notch"
};

void setup() {
  Serial.begin(115200);
  Serial.println("KoeKit Filter Sweep Example");
  
  // Setup pins
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize KoeKit
  if (!KoeKit::begin(22050, 1)) {
    Serial.println("Failed to initialize KoeKit!");
    while (1);
  }
  
  Serial.println("KoeKit initialized successfully");
  
  // Configure oscillator
  sawOsc.setFrequency(110.0f);  // Low bass note for filter demonstration
  sawOsc.setAmplitude(0.6f);
  
  // Configure filter with high resonance
  filter.setParams(cutoffFreq, 4.0f);  // High resonance for dramatic effect
  
  // Set audio callback
  KoeKit::setAudioCallback([]() -> float {
    updateFilterSweep();
    
    float sample = sawOsc.process();
    filter.process(sample);
    
    // Return appropriate filter output based on mode
    switch (filterMode) {
      case 0: return filter.getLowPass();
      case 1: return filter.getHighPass();
      case 2: return filter.getBandPass();
      case 3: return filter.getNotch();
      default: return sample;
    }
  });
  
  Serial.println("Filter sweep running...");
  Serial.println("Button on pin 2 changes filter mode");
  Serial.println("Potentiometer on A0 controls resonance");
  Serial.print("Current mode: ");
  Serial.println(filterModeNames[filterMode]);
}

void loop() {
  // Check for mode button press
  bool currentButtonState = digitalRead(MODE_BUTTON_PIN);
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    // Debounce
    if (millis() - lastModeChange > 200) {
      filterMode = (filterMode + 1) % 4;
      Serial.print("Filter mode: ");
      Serial.println(filterModeNames[filterMode]);
      lastModeChange = millis();
    }
  }
  lastButtonState = currentButtonState;
  
  // Read resonance control
  updateResonanceFromPot();
  
  // Print status periodically
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    Serial.print("Cutoff: ");
    Serial.print(cutoffFreq, 0);
    Serial.print("Hz, Mode: ");
    Serial.println(filterModeNames[filterMode]);
    lastPrint = millis();
  }
  
  delay(50);
}

void updateFilterSweep() {
  // Update filter cutoff frequency
  if (sweepingUp) {
    cutoffFreq += SWEEP_SPEED;
    if (cutoffFreq >= MAX_CUTOFF) {
      cutoffFreq = MAX_CUTOFF;
      sweepingUp = false;
    }
  } else {
    cutoffFreq -= SWEEP_SPEED;
    if (cutoffFreq <= MIN_CUTOFF) {
      cutoffFreq = MIN_CUTOFF;
      sweepingUp = true;
    }
  }
}

void updateResonanceFromPot() {
  static unsigned long lastRead = 0;
  
  // Update resonance every 100ms to avoid noise
  if (millis() - lastRead > 100) {
    int potValue = analogRead(RESONANCE_POT_PIN);
    
    if (potValue > 10) {  // Only if pot is connected
      // Map potentiometer to resonance range (0.5 to 8.0)
      float resonance = map(potValue, 0, 1023, 50, 800) / 100.0f;
      filter.setParams(cutoffFreq, resonance);
    } else {
      // Use default resonance if no pot connected
      filter.setParams(cutoffFreq, 4.0f);
    }
    
    lastRead = millis();
  }
}
/**
 * @file audio_output.cpp
 * @brief Audio output implementation for KoeKit
 */

#include "audio_output.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

namespace KoeKit {
    
    // Static member initialization
    PWMAudioOutput* PWMAudioOutput::instance_ = nullptr;
    AudioEngine* AudioEngine::instance_ = nullptr;
    
    // Timer alarm number (use alarm 0)
    static constexpr uint AUDIO_ALARM_NUM = 0;
    
    //=============================================================================
    // PWMAudioOutput Implementation
    //=============================================================================
    
    PWMAudioOutput& PWMAudioOutput::getInstance() {
        if (instance_ == nullptr) {
            static PWMAudioOutput instance;
            instance_ = &instance;
        }
        return *instance_;
    }
    
    bool PWMAudioOutput::begin(uint8_t pin, uint32_t sample_rate) {
        if (timer_active_) {
            end(); // Stop current operation
        }
        
        output_pin_ = pin;
        sample_rate_ = sample_rate;
        
        // Calculate timer period
        timer_period_us_ = 1000000 / sample_rate;
        
        // Setup PWM pin
        pinMode(output_pin_, OUTPUT);
        analogWriteResolution(PWM_RESOLUTION);
        analogWriteFreq(100000); // 100kHz PWM frequency
        
        // Write center value (silence)
        analogWrite(output_pin_, PWM_CENTER);
        
        // Setup timer
        if (!setupTimer()) {
            return false;
        }
        
        timer_active_ = true;
        return true;
    }
    
    void PWMAudioOutput::setCallback(AudioCallback callback) {
        callback_ = callback;
    }
    
    void PWMAudioOutput::end() {
        if (timer_active_) {
            stopTimer();
            timer_active_ = false;
        }
        
        // Set output to center (silence)
        if (output_pin_ != 255) {
            analogWrite(output_pin_, PWM_CENTER);
        }
        
        callback_ = nullptr;
    }
    
    void PWMAudioOutput::writeSample(float sample) {
        const uint16_t pwm_value = sampleToPWM(sample);
        analogWrite(output_pin_, pwm_value);
    }
    
    uint16_t PWMAudioOutput::sampleToPWM(float sample) {
        // Clamp sample to valid range
        sample = std::clamp(sample, -1.0f, 1.0f);
        
        // Convert to PWM range
        const float scaled = (sample + 1.0f) * 0.5f; // 0.0 to 1.0
        const uint16_t pwm_value = static_cast<uint16_t>(scaled * PWM_MAX_VALUE);
        
        return std::clamp(pwm_value, static_cast<uint16_t>(0), PWM_MAX_VALUE);
    }
    
    bool PWMAudioOutput::setupTimer() {
        // Cancel any existing alarm
        cancel_alarm(AUDIO_ALARM_NUM);
        
        // Set up the alarm
        const uint64_t period_us = timer_period_us_;
        const uint64_t target_time = time_us_64() + period_us;
        
        // Set alarm with callback
        return alarm_in_us(period_us, timerISR);
    }
    
    void PWMAudioOutput::stopTimer() {
        cancel_alarm(AUDIO_ALARM_NUM);
    }
    
    void PWMAudioOutput::timerISR() {
        if (instance_ != nullptr) {
            instance_->handleTimerInterrupt();
        }
    }
    
    void PWMAudioOutput::handleTimerInterrupt() {
        // Generate next sample
        float sample = 0.0f;
        if (callback_) {
            sample = callback_();
        }
        
        // Output sample
        writeSample(sample);
        
        // Schedule next interrupt
        if (timer_active_) {
            alarm_in_us(timer_period_us_, timerISR);
        }
    }
    
    //=============================================================================
    // AudioEngine Implementation
    //=============================================================================
    
    AudioEngine& AudioEngine::getInstance() {
        if (instance_ == nullptr) {
            static AudioEngine instance;
            instance_ = &instance;
        }
        return *instance_;
    }
    
    bool AudioEngine::begin(uint32_t sample_rate, uint8_t output_pin) {
        if (initialized_) {
            end(); // Stop current operation
        }
        
        output_ = &PWMAudioOutput::getInstance();
        
        if (!output_->begin(output_pin, sample_rate)) {
            return false;
        }
        
        // Set our callback wrapper
        output_->setCallback([this]() { return audioCallback(); });
        
        initialized_ = true;
        return true;
    }
    
    void AudioEngine::setCallback(AudioCallback callback) {
        user_callback_ = callback;
    }
    
    void AudioEngine::end() {
        if (output_) {
            output_->end();
        }
        user_callback_ = nullptr;
        initialized_ = false;
    }
    
    bool AudioEngine::isActive() const {
        return initialized_ && output_ && output_->isActive();
    }
    
    uint32_t AudioEngine::getSampleRate() const {
        return output_ ? output_->getSampleRate() : 0;
    }
    
    float AudioEngine::audioCallback() {
        if (user_callback_) {
            return user_callback_();
        }
        return 0.0f; // Silence
    }
    
    //=============================================================================
    // Global Functions
    //=============================================================================
    
    bool begin(uint32_t sample_rate, uint8_t output_pin) {
        return AudioEngine::getInstance().begin(sample_rate, output_pin);
    }
    
    void setAudioCallback(AudioCallback callback) {
        AudioEngine::getInstance().setCallback(callback);
    }
    
    void end() {
        AudioEngine::getInstance().end();
    }
    
    uint32_t getSampleRate() {
        return AudioEngine::getInstance().getSampleRate();
    }
    
} // namespace KoeKit
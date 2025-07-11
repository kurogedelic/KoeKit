#pragma once

/**
 * @file audio_output.h
 * @brief Audio output implementations for KoeKit
 */

#ifndef KOEKIT_AUDIO_OUTPUT_H
#define KOEKIT_AUDIO_OUTPUT_H

#include <Arduino.h>
#include <functional>

namespace KoeKit {
    
    /**
     * @brief Audio output callback function type
     * 
     * Called at sample rate to generate audio samples.
     * Should return a float value between -1.0 and 1.0.
     */
    using AudioCallback = std::function<float()>;
    
    /**
     * @brief Simple PWM audio output
     * 
     * Basic PWM-based audio output using Arduino's analogWrite().
     * This is a simple implementation for initial testing.
     * Future versions will use PIO for better performance.
     */
    class PWMAudioOutput {
    private:
        static PWMAudioOutput* instance_;
        
        uint8_t output_pin_ = 1;
        uint32_t sample_rate_ = SAMPLE_RATE;
        AudioCallback callback_ = nullptr;
        
        // Timer variables
        volatile bool timer_active_ = false;
        uint32_t timer_period_us_ = 0;
        
        // Sample conversion
        static constexpr uint16_t PWM_RESOLUTION = 12;  // 12-bit PWM
        static constexpr uint16_t PWM_MAX_VALUE = (1 << PWM_RESOLUTION) - 1;
        static constexpr uint16_t PWM_CENTER = PWM_MAX_VALUE / 2;
        
    public:
        /**
         * @brief Initialize PWM audio output
         * @param pin PWM output pin
         * @param sample_rate Sample rate in Hz
         * @return true if initialization successful
         */
        bool begin(uint8_t pin, uint32_t sample_rate);
        
        /**
         * @brief Set audio callback function
         * @param callback Function to call for each sample
         */
        void setCallback(AudioCallback callback);
        
        /**
         * @brief Stop audio output
         */
        void end();
        
        /**
         * @brief Check if output is active
         * @return true if audio output is running
         */
        bool isActive() const { return timer_active_; }
        
        /**
         * @brief Get current sample rate
         * @return Sample rate in Hz
         */
        uint32_t getSampleRate() const { return sample_rate_; }
        
        /**
         * @brief Get output pin
         * @return PWM output pin number
         */
        uint8_t getOutputPin() const { return output_pin_; }
        
        /**
         * @brief Write single sample to PWM output
         * @param sample Sample value (-1.0 to 1.0)
         */
        void writeSample(float sample);
        
        /**
         * @brief Get singleton instance
         * @return Reference to the singleton instance
         */
        static PWMAudioOutput& getInstance();
        
    private:
        /**
         * @brief Convert float sample to PWM value
         * @param sample Float sample (-1.0 to 1.0)
         * @return PWM value (0 to PWM_MAX_VALUE)
         */
        static uint16_t sampleToPWM(float sample);
        
        /**
         * @brief Timer interrupt handler (static)
         */
        static void timerISR();
        
        /**
         * @brief Timer interrupt handler (instance method)
         */
        void handleTimerInterrupt();
        
        /**
         * @brief Setup hardware timer
         * @return true if timer setup successful
         */
        bool setupTimer();
        
        /**
         * @brief Stop hardware timer
         */
        void stopTimer();
    };
    
    /**
     * @brief Audio engine class
     * 
     * High-level interface for audio processing.
     * Manages oscillators, effects, and output.
     */
    class AudioEngine {
    private:
        static AudioEngine* instance_;
        PWMAudioOutput* output_ = nullptr;
        AudioCallback user_callback_ = nullptr;
        bool initialized_ = false;
        
    public:
        /**
         * @brief Initialize audio engine
         * @param sample_rate Sample rate in Hz
         * @param output_pin PWM output pin
         * @return true if initialization successful
         */
        bool begin(uint32_t sample_rate = SAMPLE_RATE, uint8_t output_pin = 1);
        
        /**
         * @brief Set audio processing callback
         * @param callback Function to generate audio samples
         */
        void setCallback(AudioCallback callback);
        
        /**
         * @brief Stop audio engine
         */
        void end();
        
        /**
         * @brief Check if engine is running
         * @return true if audio engine is active
         */
        bool isActive() const;
        
        /**
         * @brief Get current sample rate
         * @return Sample rate in Hz
         */
        uint32_t getSampleRate() const;
        
        /**
         * @brief Get singleton instance
         * @return Reference to the singleton instance
         */
        static AudioEngine& getInstance();
        
    private:
        /**
         * @brief Internal audio callback wrapper
         * @return Generated audio sample
         */
        float audioCallback();
    };
    
    // Global functions for easy access
    
    /**
     * @brief Initialize KoeKit audio system
     * @param sample_rate Sample rate in Hz
     * @param output_pin PWM output pin
     * @return true if initialization successful
     */
    bool begin(uint32_t sample_rate = SAMPLE_RATE, uint8_t output_pin = 1);
    
    /**
     * @brief Set audio processing callback
     * @param callback Function to generate audio samples
     */
    void setAudioCallback(AudioCallback callback);
    
    /**
     * @brief Stop KoeKit audio system
     */
    void end();
    
    /**
     * @brief Get current sample rate
     * @return Sample rate in Hz
     */
    uint32_t getSampleRate();
    
} // namespace KoeKit

#endif // KOEKIT_AUDIO_OUTPUT_H
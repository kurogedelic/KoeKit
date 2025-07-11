#pragma once

/**
 * @file envelope.h
 * @brief Envelope generator implementations for KoeKit
 */

#ifndef KOEKIT_ENVELOPE_H
#define KOEKIT_ENVELOPE_H

#include <cmath>
#include <algorithm>

namespace KoeKit {
namespace Envelope {
    
    /**
     * @brief ADSR envelope generator
     * 
     * Classic Attack-Decay-Sustain-Release envelope generator.
     * Essential for creating natural-sounding musical instruments.
     */
    class ADSR {
    public:
        /**
         * @brief Envelope stages
         */
        enum class Stage : uint8_t {
            IDLE,       ///< Envelope is inactive (output = 0)
            ATTACK,     ///< Attack phase (rising to peak)
            DECAY,      ///< Decay phase (falling to sustain level)
            SUSTAIN,    ///< Sustain phase (holding sustain level)
            RELEASE     ///< Release phase (falling to zero)
        };
        
    private:
        Stage stage_ = Stage::IDLE;
        float level_ = 0.0f;           // Current envelope level
        float target_ = 0.0f;          // Target level for current stage
        float increment_ = 0.0f;       // Per-sample increment
        
        // ADSR parameters (in seconds)
        float attack_time_ = 0.01f;
        float decay_time_ = 0.1f;
        float sustain_level_ = 0.7f;
        float release_time_ = 0.3f;
        
        float sample_rate_ = SAMPLE_RATE_F;
        
        // Pre-calculated increments for efficiency
        float attack_inc_ = 0.0f;
        float decay_inc_ = 0.0f;
        float release_inc_ = 0.0f;
        
    public:
        /**
         * @brief Set ADSR parameters
         * @param attack Attack time in seconds
         * @param decay Decay time in seconds  
         * @param sustain Sustain level (0.0 to 1.0)
         * @param release Release time in seconds
         */
        void setADSR(float attack, float decay, float sustain, float release) noexcept {
            attack_time_ = std::max(0.001f, attack);    // Minimum 1ms
            decay_time_ = std::max(0.001f, decay);
            sustain_level_ = std::clamp(sustain, 0.0f, 1.0f);
            release_time_ = std::max(0.001f, release);
            
            updateIncrements();
        }
        
        /**
         * @brief Set attack time
         * @param attack Attack time in seconds
         */
        void setAttack(float attack) noexcept {
            setADSR(attack, decay_time_, sustain_level_, release_time_);
        }
        
        /**
         * @brief Set decay time
         * @param decay Decay time in seconds
         */
        void setDecay(float decay) noexcept {
            setADSR(attack_time_, decay, sustain_level_, release_time_);
        }
        
        /**
         * @brief Set sustain level
         * @param sustain Sustain level (0.0 to 1.0)
         */
        void setSustain(float sustain) noexcept {
            setADSR(attack_time_, decay_time_, sustain, release_time_);
        }
        
        /**
         * @brief Set release time
         * @param release Release time in seconds
         */
        void setRelease(float release) noexcept {
            setADSR(attack_time_, decay_time_, sustain_level_, release);
        }
        
        /**
         * @brief Trigger note on (start envelope)
         */
        void noteOn() noexcept {
            stage_ = Stage::ATTACK;
            target_ = 1.0f;
            increment_ = attack_inc_;
        }
        
        /**
         * @brief Trigger note off (start release)
         */
        void noteOff() noexcept {
            if (stage_ != Stage::IDLE) {
                stage_ = Stage::RELEASE;
                target_ = 0.0f;
                increment_ = -release_inc_;
            }
        }
        
        /**
         * @brief Process one sample
         * @return Envelope level (0.0 to 1.0)
         */
        float process() noexcept {
            switch (stage_) {
                case Stage::IDLE:
                    level_ = 0.0f;
                    break;
                    
                case Stage::ATTACK:
                    level_ += increment_;
                    if (level_ >= 1.0f) {
                        level_ = 1.0f;
                        stage_ = Stage::DECAY;
                        target_ = sustain_level_;
                        increment_ = -decay_inc_;
                    }
                    break;
                    
                case Stage::DECAY:
                    level_ += increment_;
                    if (level_ <= sustain_level_) {
                        level_ = sustain_level_;
                        stage_ = Stage::SUSTAIN;
                        increment_ = 0.0f;
                    }
                    break;
                    
                case Stage::SUSTAIN:
                    level_ = sustain_level_;
                    break;
                    
                case Stage::RELEASE:
                    level_ += increment_;
                    if (level_ <= 0.0f) {
                        level_ = 0.0f;
                        stage_ = Stage::IDLE;
                        increment_ = 0.0f;
                    }
                    break;
            }
            
            return level_;
        }
        
        /**
         * @brief Process input sample with envelope applied
         * @param input Input sample
         * @return Enveloped output sample
         */
        float process(float input) noexcept {
            return input * process();
        }
        
        /**
         * @brief Check if envelope is active
         * @return true if envelope is generating output
         */
        bool isActive() const noexcept {
            return stage_ != Stage::IDLE;
        }
        
        /**
         * @brief Get current envelope stage
         * @return Current stage
         */
        Stage getStage() const noexcept {
            return stage_;
        }
        
        /**
         * @brief Get current envelope level
         * @return Current level (0.0 to 1.0)
         */
        float getLevel() const noexcept {
            return level_;
        }
        
        /**
         * @brief Reset envelope to idle state
         */
        void reset() noexcept {
            stage_ = Stage::IDLE;
            level_ = 0.0f;
            target_ = 0.0f;
            increment_ = 0.0f;
        }
        
        /**
         * @brief Set sample rate
         * @param sample_rate Sample rate in Hz
         */
        void setSampleRate(float sample_rate) noexcept {
            sample_rate_ = sample_rate;
            updateIncrements();
        }
        
        // Getters for current parameters
        float getAttack() const noexcept { return attack_time_; }
        float getDecay() const noexcept { return decay_time_; }
        float getSustain() const noexcept { return sustain_level_; }
        float getRelease() const noexcept { return release_time_; }
        
    private:
        void updateIncrements() noexcept {
            attack_inc_ = 1.0f / (attack_time_ * sample_rate_);
            decay_inc_ = (1.0f - sustain_level_) / (decay_time_ * sample_rate_);
            release_inc_ = sustain_level_ / (release_time_ * sample_rate_);
        }
    };
    
    /**
     * @brief Simple AR (Attack-Release) envelope
     * 
     * Simplified envelope for percussive sounds or simple effects.
     */
    class AR {
    public:
        enum class Stage : uint8_t {
            IDLE,
            ATTACK, 
            RELEASE
        };
        
    private:
        Stage stage_ = Stage::IDLE;
        float level_ = 0.0f;
        float increment_ = 0.0f;
        
        float attack_time_ = 0.01f;
        float release_time_ = 0.3f;
        float sample_rate_ = SAMPLE_RATE_F;
        
        float attack_inc_ = 0.0f;
        float release_inc_ = 0.0f;
        
    public:
        /**
         * @brief Set AR parameters
         * @param attack Attack time in seconds
         * @param release Release time in seconds
         */
        void setAR(float attack, float release) noexcept {
            attack_time_ = std::max(0.001f, attack);
            release_time_ = std::max(0.001f, release);
            updateIncrements();
        }
        
        /**
         * @brief Trigger envelope
         */
        void trigger() noexcept {
            stage_ = Stage::ATTACK;
            increment_ = attack_inc_;
        }
        
        /**
         * @brief Process one sample
         * @return Envelope level (0.0 to 1.0)
         */
        float process() noexcept {
            switch (stage_) {
                case Stage::IDLE:
                    level_ = 0.0f;
                    break;
                    
                case Stage::ATTACK:
                    level_ += increment_;
                    if (level_ >= 1.0f) {
                        level_ = 1.0f;
                        stage_ = Stage::RELEASE;
                        increment_ = -release_inc_;
                    }
                    break;
                    
                case Stage::RELEASE:
                    level_ += increment_;
                    if (level_ <= 0.0f) {
                        level_ = 0.0f;
                        stage_ = Stage::IDLE;
                        increment_ = 0.0f;
                    }
                    break;
            }
            
            return level_;
        }
        
        /**
         * @brief Process input sample with envelope applied
         * @param input Input sample
         * @return Enveloped output sample
         */
        float process(float input) noexcept {
            return input * process();
        }
        
        bool isActive() const noexcept { return stage_ != Stage::IDLE; }
        Stage getStage() const noexcept { return stage_; }
        float getLevel() const noexcept { return level_; }
        
        void reset() noexcept {
            stage_ = Stage::IDLE;
            level_ = 0.0f;
            increment_ = 0.0f;
        }
        
        void setSampleRate(float sample_rate) noexcept {
            sample_rate_ = sample_rate;
            updateIncrements();
        }
        
    private:
        void updateIncrements() noexcept {
            attack_inc_ = 1.0f / (attack_time_ * sample_rate_);
            release_inc_ = 1.0f / (release_time_ * sample_rate_);
        }
    };
    
    /**
     * @brief Low Frequency Oscillator (LFO)
     * 
     * Slow oscillator for modulating other parameters.
     * Can generate various waveforms at low frequencies.
     */
    class LFO {
    public:
        enum class Waveform : uint8_t {
            SINE,
            TRIANGLE,
            SAWTOOTH,
            SQUARE,
            SAMPLE_HOLD,
            NOISE
        };
        
    private:
        float phase_ = 0.0f;
        float frequency_ = 1.0f;        // Hz
        float amplitude_ = 1.0f;
        float offset_ = 0.0f;
        Waveform waveform_ = Waveform::SINE;
        float sample_rate_ = SAMPLE_RATE_F;
        
        // For sample & hold and noise
        float hold_value_ = 0.0f;
        uint32_t noise_state_ = 1;
        
    public:
        /**
         * @brief Set LFO frequency
         * @param frequency Frequency in Hz (typically 0.1 to 20 Hz)
         */
        void setFrequency(float frequency) noexcept {
            frequency_ = std::clamp(frequency, 0.001f, 100.0f);
        }
        
        /**
         * @brief Set LFO amplitude
         * @param amplitude Amplitude (0.0 to 1.0)
         */
        void setAmplitude(float amplitude) noexcept {
            amplitude_ = std::clamp(amplitude, 0.0f, 1.0f);
        }
        
        /**
         * @brief Set DC offset
         * @param offset DC offset (-1.0 to 1.0)
         */
        void setOffset(float offset) noexcept {
            offset_ = std::clamp(offset, -1.0f, 1.0f);
        }
        
        /**
         * @brief Set waveform type
         * @param waveform Waveform type
         */
        void setWaveform(Waveform waveform) noexcept {
            waveform_ = waveform;
        }
        
        /**
         * @brief Process one sample
         * @return LFO output (-1.0 to 1.0, plus offset)
         */
        float process() noexcept {
            float output = 0.0f;
            
            switch (waveform_) {
                case Waveform::SINE:
                    output = std::sin(2.0f * M_PI * phase_);
                    break;
                    
                case Waveform::TRIANGLE:
                    if (phase_ < 0.5f) {
                        output = 4.0f * phase_ - 1.0f;
                    } else {
                        output = 3.0f - 4.0f * phase_;
                    }
                    break;
                    
                case Waveform::SAWTOOTH:
                    output = 2.0f * phase_ - 1.0f;
                    break;
                    
                case Waveform::SQUARE:
                    output = (phase_ < 0.5f) ? 1.0f : -1.0f;
                    break;
                    
                case Waveform::SAMPLE_HOLD:
                    if (phase_ < (frequency_ / sample_rate_)) {
                        // Generate new random value at zero crossing
                        noise_state_ ^= noise_state_ << 13;
                        noise_state_ ^= noise_state_ >> 17;
                        noise_state_ ^= noise_state_ << 5;
                        hold_value_ = (static_cast<float>(noise_state_) / UINT32_MAX) * 2.0f - 1.0f;
                    }
                    output = hold_value_;
                    break;
                    
                case Waveform::NOISE:
                    noise_state_ ^= noise_state_ << 13;
                    noise_state_ ^= noise_state_ >> 17;
                    noise_state_ ^= noise_state_ << 5;
                    output = (static_cast<float>(noise_state_) / UINT32_MAX) * 2.0f - 1.0f;
                    break;
            }
            
            // Update phase
            phase_ += frequency_ / sample_rate_;
            if (phase_ >= 1.0f) {
                phase_ -= 1.0f;
            }
            
            return output * amplitude_ + offset_;
        }
        
        /**
         * @brief Reset LFO phase
         */
        void reset() noexcept {
            phase_ = 0.0f;
            hold_value_ = 0.0f;
        }
        
        /**
         * @brief Set sample rate
         * @param sample_rate Sample rate in Hz
         */
        void setSampleRate(float sample_rate) noexcept {
            sample_rate_ = sample_rate;
        }
        
        // Getters
        float getFrequency() const noexcept { return frequency_; }
        float getAmplitude() const noexcept { return amplitude_; }
        float getOffset() const noexcept { return offset_; }
        Waveform getWaveform() const noexcept { return waveform_; }
    };
    
} // namespace Envelope
} // namespace KoeKit

#endif // KOEKIT_ENVELOPE_H
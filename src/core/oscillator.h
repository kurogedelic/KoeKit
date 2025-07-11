#pragma once

/**
 * @file oscillator.h
 * @brief Oscillator implementations for KoeKit
 */

#ifndef KOEKIT_OSCILLATOR_H
#define KOEKIT_OSCILLATOR_H

#include "wavetable_generator.h"
#include "../wavetables/basic.h"

namespace KoeKit {
    
    /**
     * @brief Phase accumulator for oscillators
     * 
     * Provides precise phase tracking with frequency control.
     * Uses double precision for phase to minimize drift.
     */
    class PhaseAccumulator {
    private:
        double phase_ = 0.0;
        double increment_ = 0.0;
        float sample_rate_ = SAMPLE_RATE_F;
        
    public:
        /**
         * @brief Set oscillator frequency
         * @param frequency Frequency in Hz
         */
        void setFrequency(float frequency) noexcept {
            increment_ = static_cast<double>(frequency) / sample_rate_;
        }
        
        /**
         * @brief Set sample rate
         * @param sample_rate Sample rate in Hz
         */
        void setSampleRate(float sample_rate) noexcept {
            const float freq = getCurrentFrequency();
            sample_rate_ = sample_rate;
            setFrequency(freq);
        }
        
        /**
         * @brief Get current frequency
         * @return Frequency in Hz
         */
        float getCurrentFrequency() const noexcept {
            return static_cast<float>(increment_ * sample_rate_);
        }
        
        /**
         * @brief Advance phase and get current phase value
         * @return Phase value (0.0 to 1.0)
         */
        float tick() noexcept {
            phase_ += increment_;
            if (phase_ >= 1.0) {
                phase_ -= 1.0;
            }
            return static_cast<float>(phase_);
        }
        
        /**
         * @brief Reset phase to zero
         */
        void reset() noexcept {
            phase_ = 0.0;
        }
        
        /**
         * @brief Set phase directly
         * @param phase Phase value (0.0 to 1.0)
         */
        void setPhase(float phase) noexcept {
            phase_ = static_cast<double>(phase);
            while (phase_ >= 1.0) phase_ -= 1.0;
            while (phase_ < 0.0) phase_ += 1.0;
        }
        
        /**
         * @brief Get current phase without advancing
         * @return Current phase (0.0 to 1.0)
         */
        float getPhase() const noexcept {
            return static_cast<float>(phase_);
        }
    };
    
    /**
     * @brief Wavetable oscillator
     * 
     * High-quality oscillator using wavetable lookup with linear interpolation.
     * Template parameter allows compile-time optimization for specific table sizes.
     */
    template<size_t TABLE_SIZE>
    class WavetableOscillator {
    private:
        PhaseAccumulator phase_;
        const Wavetable<TABLE_SIZE>* wavetable_;
        float amplitude_ = 1.0f;
        
    public:
        /**
         * @brief Construct oscillator with wavetable
         * @param wavetable Reference to wavetable
         */
        explicit WavetableOscillator(const Wavetable<TABLE_SIZE>& wavetable) 
            : wavetable_(&wavetable) {}
        
        /**
         * @brief Set oscillator frequency
         * @param frequency Frequency in Hz
         */
        void setFrequency(float frequency) noexcept {
            phase_.setFrequency(frequency);
        }
        
        /**
         * @brief Set oscillator amplitude
         * @param amplitude Amplitude (0.0 to 1.0)
         */
        void setAmplitude(float amplitude) noexcept {
            amplitude_ = std::clamp(amplitude, 0.0f, 1.0f);
        }
        
        /**
         * @brief Set phase offset
         * @param phase Phase (0.0 to 1.0)
         */
        void setPhase(float phase) noexcept {
            phase_.setPhase(phase);
        }
        
        /**
         * @brief Change wavetable
         * @param wavetable Reference to new wavetable
         */
        void setWavetable(const Wavetable<TABLE_SIZE>& wavetable) noexcept {
            wavetable_ = &wavetable;
        }
        
        /**
         * @brief Process one sample
         * @return Output sample (-1.0 to 1.0)
         */
        float process() noexcept {
            const float phase = phase_.tick();
            const float table_index = phase * TABLE_SIZE;
            return wavetable_->getInterpolated(table_index) * amplitude_;
        }
        
        /**
         * @brief Reset oscillator state
         */
        void reset() noexcept {
            phase_.reset();
        }
        
        /**
         * @brief Set sample rate
         * @param sample_rate Sample rate in Hz
         */
        void setSampleRate(float sample_rate) noexcept {
            phase_.setSampleRate(sample_rate);
        }
        
        /**
         * @brief Get current frequency
         * @return Frequency in Hz
         */
        float getFrequency() const noexcept {
            return phase_.getCurrentFrequency();
        }
        
        /**
         * @brief Get current amplitude
         * @return Amplitude (0.0 to 1.0)
         */
        float getAmplitude() const noexcept {
            return amplitude_;
        }
    };
    
    /**
     * @brief Convenient type alias for standard wavetable oscillator
     */
    using Oscillator = WavetableOscillator<WAVETABLE_SIZE>;
    
    /**
     * @brief Simple noise generator
     * 
     * Fast pseudo-random noise generator using XorShift algorithm.
     */
    class NoiseGenerator {
    private:
        uint32_t state_ = 1;
        float amplitude_ = 1.0f;
        
    public:
        /**
         * @brief Construct noise generator with optional seed
         * @param seed Random seed (default: 1)
         */
        explicit NoiseGenerator(uint32_t seed = 1) : state_(seed == 0 ? 1 : seed) {}
        
        /**
         * @brief Set noise amplitude
         * @param amplitude Amplitude (0.0 to 1.0)
         */
        void setAmplitude(float amplitude) noexcept {
            amplitude_ = std::clamp(amplitude, 0.0f, 1.0f);
        }
        
        /**
         * @brief Process one sample
         * @return Random sample (-1.0 to 1.0)
         */
        float process() noexcept {
            // XorShift algorithm
            state_ ^= state_ << 13;
            state_ ^= state_ >> 17;
            state_ ^= state_ << 5;
            
            // Convert to float range [-1.0, 1.0]
            const float random = static_cast<float>(state_) / static_cast<float>(UINT32_MAX);
            return (random * 2.0f - 1.0f) * amplitude_;
        }
        
        /**
         * @brief Reset with new seed
         * @param seed New random seed
         */
        void reset(uint32_t seed = 1) noexcept {
            state_ = (seed == 0) ? 1 : seed;
        }
        
        /**
         * @brief Get current amplitude
         * @return Amplitude (0.0 to 1.0)
         */
        float getAmplitude() const noexcept {
            return amplitude_;
        }
        
        // Dummy methods for interface compatibility
        void setFrequency(float) noexcept {}
        void setSampleRate(float) noexcept {}
        float getFrequency() const noexcept { return 0.0f; }
    };
    
    /**
     * @brief Create oscillator with basic waveform
     * @param waveform Basic waveform type
     * @return Configured oscillator
     */
    inline Oscillator createOscillator(Wavetables::Basic::Waveform waveform) {
        return Oscillator(Wavetables::Basic::getWavetable(waveform));
    }
    
} // namespace KoeKit

#endif // KOEKIT_OSCILLATOR_H
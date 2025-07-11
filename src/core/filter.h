#pragma once

/**
 * @file filter.h
 * @brief Digital filter implementations for KoeKit
 */

#ifndef KOEKIT_FILTER_H
#define KOEKIT_FILTER_H

#include <cmath>
#include <algorithm>

namespace KoeKit {
namespace Filter {
    
    /**
     * @brief One-pole filter (first-order)
     * 
     * Simple and efficient filter suitable for basic low-pass and high-pass filtering.
     * Uses minimal CPU and memory resources.
     */
    class OnePole {
    private:
        float y1_ = 0.0f;           // Previous output
        float a0_ = 1.0f;           // Feed-forward coefficient
        float b1_ = 0.0f;           // Feedback coefficient
        float sample_rate_ = SAMPLE_RATE_F;
        float cutoff_ = 1000.0f;
        
    public:
        /**
         * @brief Set filter cutoff frequency
         * @param cutoff Cutoff frequency in Hz
         */
        void setCutoff(float cutoff) noexcept {
            cutoff_ = std::clamp(cutoff, 1.0f, sample_rate_ * 0.49f);
            updateCoefficients();
        }
        
        /**
         * @brief Set sample rate
         * @param sample_rate Sample rate in Hz
         */
        void setSampleRate(float sample_rate) noexcept {
            sample_rate_ = sample_rate;
            updateCoefficients();
        }
        
        /**
         * @brief Low-pass filter processing
         * @param input Input sample
         * @return Filtered output sample
         */
        float processLPF(float input) noexcept {
            y1_ = a0_ * input + b1_ * y1_;
            return y1_;
        }
        
        /**
         * @brief High-pass filter processing
         * @param input Input sample  
         * @return Filtered output sample
         */
        float processHPF(float input) noexcept {
            const float lpf_out = processLPF(input);
            return input - lpf_out;
        }
        
        /**
         * @brief Reset filter state
         */
        void reset() noexcept {
            y1_ = 0.0f;
        }
        
        /**
         * @brief Get current cutoff frequency
         * @return Cutoff frequency in Hz
         */
        float getCutoff() const noexcept {
            return cutoff_;
        }
        
    private:
        void updateCoefficients() noexcept {
            const float omega = 2.0f * M_PI * cutoff_ / sample_rate_;
            const float alpha = 1.0f - std::exp(-omega);
            a0_ = alpha;
            b1_ = 1.0f - alpha;
        }
    };
    
    /**
     * @brief State Variable Filter (SVF)
     * 
     * High-quality 2-pole filter providing simultaneous low-pass, high-pass,
     * band-pass, and notch outputs. Excellent for musical applications.
     */
    class StateVariable {
    private:
        float low_ = 0.0f;          // Low-pass output
        float band_ = 0.0f;         // Band-pass output  
        float high_ = 0.0f;         // High-pass output
        float f_ = 0.0f;            // Frequency coefficient
        float q_ = 1.0f;            // Q (resonance) coefficient
        float sample_rate_ = SAMPLE_RATE_F;
        float cutoff_ = 1000.0f;
        float resonance_ = 0.7f;
        
    public:
        /**
         * @brief Set filter parameters
         * @param cutoff Cutoff frequency in Hz
         * @param resonance Resonance (Q) factor (0.1 to 10.0)
         */
        void setParams(float cutoff, float resonance) noexcept {
            cutoff_ = std::clamp(cutoff, 1.0f, sample_rate_ * 0.45f);
            resonance_ = std::clamp(resonance, 0.1f, 10.0f);
            updateCoefficients();
        }
        
        /**
         * @brief Set cutoff frequency only
         * @param cutoff Cutoff frequency in Hz
         */
        void setCutoff(float cutoff) noexcept {
            setParams(cutoff, resonance_);
        }
        
        /**
         * @brief Set resonance only
         * @param resonance Resonance factor (0.1 to 10.0)
         */
        void setResonance(float resonance) noexcept {
            setParams(cutoff_, resonance);
        }
        
        /**
         * @brief Set sample rate
         * @param sample_rate Sample rate in Hz
         */
        void setSampleRate(float sample_rate) noexcept {
            sample_rate_ = sample_rate;
            updateCoefficients();
        }
        
        /**
         * @brief Process input sample through filter
         * @param input Input sample
         */
        void process(float input) noexcept {
            low_ += f_ * band_;
            high_ = input - low_ - q_ * band_;
            band_ += f_ * high_;
            
            // Prevent denormals
            if (std::abs(low_) < 1e-10f) low_ = 0.0f;
            if (std::abs(band_) < 1e-10f) band_ = 0.0f;
            if (std::abs(high_) < 1e-10f) high_ = 0.0f;
        }
        
        /**
         * @brief Get low-pass output
         * @return Low-pass filtered sample
         */
        float getLowPass() const noexcept { return low_; }
        
        /**
         * @brief Get high-pass output
         * @return High-pass filtered sample
         */
        float getHighPass() const noexcept { return high_; }
        
        /**
         * @brief Get band-pass output
         * @return Band-pass filtered sample
         */
        float getBandPass() const noexcept { return band_; }
        
        /**
         * @brief Get notch (band-reject) output
         * @return Notch filtered sample
         */
        float getNotch() const noexcept { return low_ + high_; }
        
        /**
         * @brief Reset filter state
         */
        void reset() noexcept {
            low_ = band_ = high_ = 0.0f;
        }
        
        /**
         * @brief Get current cutoff frequency
         * @return Cutoff frequency in Hz
         */
        float getCutoff() const noexcept {
            return cutoff_;
        }
        
        /**
         * @brief Get current resonance
         * @return Resonance factor
         */
        float getResonance() const noexcept {
            return resonance_;
        }
        
    private:
        void updateCoefficients() noexcept {
            f_ = 2.0f * std::sin(M_PI * cutoff_ / sample_rate_);
            q_ = 1.0f / resonance_;
            
            // Clamp to stable range
            f_ = std::clamp(f_, 0.0f, 1.9f);
            q_ = std::clamp(q_, 0.01f, 2.0f);
        }
    };
    
    /**
     * @brief Biquad filter implementation
     * 
     * General-purpose biquad filter that can implement various filter types
     * including Butterworth, Chebyshev, and other standard responses.
     */
    class Biquad {
    private:
        // Direct Form I implementation
        float x1_ = 0.0f, x2_ = 0.0f;  // Input history
        float y1_ = 0.0f, y2_ = 0.0f;  // Output history
        
        // Coefficients
        float b0_ = 1.0f, b1_ = 0.0f, b2_ = 0.0f;  // Numerator
        float a1_ = 0.0f, a2_ = 0.0f;               // Denominator (a0 = 1)
        
        float sample_rate_ = SAMPLE_RATE_F;
        
    public:
        /**
         * @brief Set biquad coefficients directly
         * @param b0 b1 b2 Numerator coefficients
         * @param a1 a2 Denominator coefficients (a0 assumed to be 1)
         */
        void setCoefficients(float b0, float b1, float b2, float a1, float a2) noexcept {
            b0_ = b0; b1_ = b1; b2_ = b2;
            a1_ = a1; a2_ = a2;
        }
        
        /**
         * @brief Configure as low-pass filter (Butterworth)
         * @param cutoff Cutoff frequency in Hz
         */
        void setLowPass(float cutoff) noexcept {
            const float omega = 2.0f * M_PI * cutoff / sample_rate_;
            const float sin_omega = std::sin(omega);
            const float cos_omega = std::cos(omega);
            const float alpha = sin_omega / (2.0f * 0.7071f); // Q = 0.7071 (Butterworth)
            
            const float a0 = 1.0f + alpha;
            b0_ = (1.0f - cos_omega) / (2.0f * a0);
            b1_ = (1.0f - cos_omega) / a0;
            b2_ = (1.0f - cos_omega) / (2.0f * a0);
            a1_ = (-2.0f * cos_omega) / a0;
            a2_ = (1.0f - alpha) / a0;
        }
        
        /**
         * @brief Configure as high-pass filter (Butterworth)
         * @param cutoff Cutoff frequency in Hz
         */
        void setHighPass(float cutoff) noexcept {
            const float omega = 2.0f * M_PI * cutoff / sample_rate_;
            const float sin_omega = std::sin(omega);
            const float cos_omega = std::cos(omega);
            const float alpha = sin_omega / (2.0f * 0.7071f);
            
            const float a0 = 1.0f + alpha;
            b0_ = (1.0f + cos_omega) / (2.0f * a0);
            b1_ = -(1.0f + cos_omega) / a0;
            b2_ = (1.0f + cos_omega) / (2.0f * a0);
            a1_ = (-2.0f * cos_omega) / a0;
            a2_ = (1.0f - alpha) / a0;
        }
        
        /**
         * @brief Configure as band-pass filter
         * @param center Center frequency in Hz
         * @param bandwidth Bandwidth in Hz
         */
        void setBandPass(float center, float bandwidth) noexcept {
            const float omega = 2.0f * M_PI * center / sample_rate_;
            const float sin_omega = std::sin(omega);
            const float cos_omega = std::cos(omega);
            const float alpha = sin_omega * std::sinh(std::log(2.0f) / 2.0f * bandwidth * omega / sin_omega);
            
            const float a0 = 1.0f + alpha;
            b0_ = alpha / a0;
            b1_ = 0.0f;
            b2_ = -alpha / a0;
            a1_ = (-2.0f * cos_omega) / a0;
            a2_ = (1.0f - alpha) / a0;
        }
        
        /**
         * @brief Process input sample
         * @param input Input sample
         * @return Filtered output sample
         */
        float process(float input) noexcept {
            // Direct Form I biquad
            const float output = b0_ * input + b1_ * x1_ + b2_ * x2_ - a1_ * y1_ - a2_ * y2_;
            
            // Update history
            x2_ = x1_;
            x1_ = input;
            y2_ = y1_;
            y1_ = output;
            
            return output;
        }
        
        /**
         * @brief Reset filter state
         */
        void reset() noexcept {
            x1_ = x2_ = y1_ = y2_ = 0.0f;
        }
        
        /**
         * @brief Set sample rate
         * @param sample_rate Sample rate in Hz
         */
        void setSampleRate(float sample_rate) noexcept {
            sample_rate_ = sample_rate;
        }
    };
    
    /**
     * @brief Simple DC blocking filter
     * 
     * High-pass filter with very low cutoff frequency to remove DC offset.
     */
    class DCBlocker {
    private:
        float x1_ = 0.0f;
        float y1_ = 0.0f;
        static constexpr float POLE = 0.995f;  // ~3Hz cutoff at 44.1kHz
        
    public:
        /**
         * @brief Process input sample
         * @param input Input sample
         * @return DC-blocked output sample
         */
        float process(float input) noexcept {
            const float output = input - x1_ + POLE * y1_;
            x1_ = input;
            y1_ = output;
            return output;
        }
        
        /**
         * @brief Reset filter state
         */
        void reset() noexcept {
            x1_ = y1_ = 0.0f;
        }
    };
    
} // namespace Filter
} // namespace KoeKit

#endif // KOEKIT_FILTER_H
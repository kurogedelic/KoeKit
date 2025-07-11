#pragma once

/**
 * @file wavetable_generator.h
 * @brief Compile-time wavetable generation system
 */

#ifndef KOEKIT_WAVETABLE_GENERATOR_H
#define KOEKIT_WAVETABLE_GENERATOR_H

#include <array>
#include <cmath>
#include <algorithm>
#include <functional>

namespace KoeKit {
    
    /**
     * @brief Sample type for wavetables (16-bit signed integer)
     */
    using WavetableSample = int16_t;
    
    /**
     * @brief Maximum sample value for 16-bit samples
     */
    constexpr float SAMPLE_SCALE = 32767.0f;
    
    /**
     * @brief Wavetable container with compile-time generation
     * @tparam SIZE Number of samples in the wavetable
     */
    template<size_t SIZE>
    class Wavetable {
    public:
        using SampleArray = std::array<WavetableSample, SIZE>;
        
    private:
        SampleArray samples_;
        
    public:
        /**
         * @brief Construct wavetable from sample array
         * @param samples Pre-computed sample array
         */
        constexpr Wavetable(const SampleArray& samples) : samples_(samples) {}
        
        /**
         * @brief Get sample at exact index (no interpolation)
         * @param index Sample index (will be wrapped)
         * @return Sample value (-32768 to 32767)
         */
        constexpr WavetableSample getSample(size_t index) const noexcept {
            return samples_[index % SIZE];
        }
        
        /**
         * @brief Get interpolated sample at fractional index
         * @param index Fractional sample index
         * @return Interpolated sample value (-1.0 to 1.0)
         */
        float getInterpolated(float index) const noexcept {
            // Wrap index to table size
            while (index >= SIZE) index -= SIZE;
            while (index < 0) index += SIZE;
            
            const auto i1 = static_cast<size_t>(index);
            const auto i2 = (i1 + 1) % SIZE;
            const auto frac = index - static_cast<float>(i1);
            
            const auto s1 = static_cast<float>(samples_[i1]);
            const auto s2 = static_cast<float>(samples_[i2]);
            
            return (s1 + frac * (s2 - s1)) / SAMPLE_SCALE;
        }
        
        /**
         * @brief Get table size
         * @return Number of samples in the table
         */
        constexpr size_t size() const noexcept { return SIZE; }
        
        /**
         * @brief Direct access to sample array
         * @return Reference to internal sample array
         */
        constexpr const SampleArray& data() const noexcept { return samples_; }
    };
    
    /**
     * @brief Generate wavetable from formula function
     * @tparam SIZE Number of samples to generate
     * @param generator Function that takes sample index and returns float value (-1.0 to 1.0)
     * @return Generated wavetable
     */
    template<size_t SIZE>
    constexpr auto generateWavetable(std::function<float(size_t)> generator) {
        typename Wavetable<SIZE>::SampleArray samples{};
        
        for (size_t i = 0; i < SIZE; ++i) {
            const float value = std::clamp(generator(i), -1.0f, 1.0f);
            samples[i] = static_cast<WavetableSample>(value * SAMPLE_SCALE);
        }
        
        return Wavetable<SIZE>(samples);
    }
    
    /**
     * @brief Generate wavetable from lambda (compile-time friendly)
     * @tparam SIZE Number of samples to generate
     * @tparam Generator Lambda or function object type
     * @param generator Callable that takes sample index and returns float value
     * @return Generated wavetable
     */
    template<size_t SIZE, typename Generator>
    constexpr auto makeWavetable(Generator generator) {
        typename Wavetable<SIZE>::SampleArray samples{};
        
        for (size_t i = 0; i < SIZE; ++i) {
            const float value = std::clamp(generator(i), -1.0f, 1.0f);
            samples[i] = static_cast<WavetableSample>(value * SAMPLE_SCALE);
        }
        
        return Wavetable<SIZE>(samples);
    }
    
    /**
     * @brief Generate wavetable from sample array
     * @tparam SIZE Number of samples
     * @param samples Array of float samples (-1.0 to 1.0)
     * @return Generated wavetable
     */
    template<size_t SIZE>
    constexpr auto makeWavetable(const std::array<float, SIZE>& samples) {
        typename Wavetable<SIZE>::SampleArray int_samples{};
        
        for (size_t i = 0; i < SIZE; ++i) {
            const float value = std::clamp(samples[i], -1.0f, 1.0f);
            int_samples[i] = static_cast<WavetableSample>(value * SAMPLE_SCALE);
        }
        
        return Wavetable<SIZE>(int_samples);
    }
    
    /**
     * @brief Collection of multiple wavetables
     * @tparam NumWaves Number of wavetables
     * @tparam WaveSize Samples per wavetable
     */
    template<size_t NumWaves, size_t WaveSize>
    class WavetableBank {
    public:
        using WaveArray = std::array<Wavetable<WaveSize>, NumWaves>;
        
    private:
        WaveArray waves_;
        
    public:
        constexpr WavetableBank(const WaveArray& waves) : waves_(waves) {}
        
        constexpr const Wavetable<WaveSize>& getWave(size_t index) const noexcept {
            return waves_[index % NumWaves];
        }
        
        constexpr size_t numWaves() const noexcept { return NumWaves; }
        constexpr size_t waveSize() const noexcept { return WaveSize; }
    };
}

#endif // KOEKIT_WAVETABLE_GENERATOR_H
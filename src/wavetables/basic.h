#pragma once

/**
 * @file basic.h
 * @brief Basic waveform tables (automatically included with KoeKit.h)
 */

#ifndef KOEKIT_WAVETABLES_BASIC_H
#define KOEKIT_WAVETABLES_BASIC_H

#include "../core/wavetable_generator.h"
#include <cmath>

namespace KoeKit {
namespace Wavetables {
namespace Basic {
    
    constexpr size_t BASIC_TABLE_SIZE = KOEKIT_WAVETABLE_SIZE;
    
    /**
     * @brief Generate sine wave table
     */
    constexpr auto makeSineTable() {
        return makeWavetable<BASIC_TABLE_SIZE>([](size_t i) -> float {
            const float phase = 2.0f * M_PI * static_cast<float>(i) / BASIC_TABLE_SIZE;
            return std::sin(phase);
        });
    }
    
    /**
     * @brief Generate sawtooth wave table
     */
    constexpr auto makeSawTable() {
        return makeWavetable<BASIC_TABLE_SIZE>([](size_t i) -> float {
            return 2.0f * static_cast<float>(i) / (BASIC_TABLE_SIZE - 1) - 1.0f;
        });
    }
    
    /**
     * @brief Generate square wave table
     */
    constexpr auto makeSquareTable() {
        return makeWavetable<BASIC_TABLE_SIZE>([](size_t i) -> float {
            return (i < BASIC_TABLE_SIZE / 2) ? 1.0f : -1.0f;
        });
    }
    
    /**
     * @brief Generate triangle wave table
     */
    constexpr auto makeTriangleTable() {
        return makeWavetable<BASIC_TABLE_SIZE>([](size_t i) -> float {
            if (i < BASIC_TABLE_SIZE / 2) {
                return 4.0f * static_cast<float>(i) / BASIC_TABLE_SIZE - 1.0f;
            } else {
                return 3.0f - 4.0f * static_cast<float>(i) / BASIC_TABLE_SIZE;
            }
        });
    }
    
    /**
     * @brief Generate soft sawtooth with reduced harmonics
     */
    constexpr auto makeSoftSawTable() {
        return makeWavetable<BASIC_TABLE_SIZE>([](size_t i) -> float {
            const float phase = 2.0f * M_PI * static_cast<float>(i) / BASIC_TABLE_SIZE;
            float result = 0.0f;
            
            // Band-limited sawtooth (first 8 harmonics)
            for (int harmonic = 1; harmonic <= 8; ++harmonic) {
                result += std::sin(harmonic * phase) / harmonic;
            }
            
            return result * 0.3f; // Scale down to prevent clipping
        });
    }
    
    /**
     * @brief Generate pulse wave with 25% duty cycle
     */
    constexpr auto makePulseTable() {
        return makeWavetable<BASIC_TABLE_SIZE>([](size_t i) -> float {
            return (i < BASIC_TABLE_SIZE / 4) ? 1.0f : -1.0f;
        });
    }
    
    // Pre-computed wavetables (available at compile time)
    inline constexpr auto SINE = makeSineTable();
    inline constexpr auto SAW = makeSawTable();
    inline constexpr auto SQUARE = makeSquareTable();
    inline constexpr auto TRIANGLE = makeTriangleTable();
    inline constexpr auto SOFT_SAW = makeSoftSawTable();
    inline constexpr auto PULSE = makePulseTable();
    
    /**
     * @brief Basic waveform bank containing all basic waves
     */
    constexpr auto makeBasicBank() {
        typename WavetableBank<6, BASIC_TABLE_SIZE>::WaveArray waves = {{
            SINE, SAW, SQUARE, TRIANGLE, SOFT_SAW, PULSE
        }};
        return WavetableBank<6, BASIC_TABLE_SIZE>(waves);
    }
    
    inline constexpr auto BASIC_BANK = makeBasicBank();
    
    /**
     * @brief Waveform enumeration for easy access
     */
    enum class Waveform : uint8_t {
        SINE = 0,
        SAW = 1,
        SQUARE = 2,
        TRIANGLE = 3,
        SOFT_SAW = 4,
        PULSE = 5
    };
    
    /**
     * @brief Get wavetable by waveform type
     * @param waveform Waveform enumeration
     * @return Reference to the corresponding wavetable
     */
    constexpr const auto& getWavetable(Waveform waveform) {
        return BASIC_BANK.getWave(static_cast<size_t>(waveform));
    }
    
} // namespace Basic
} // namespace Wavetables
} // namespace KoeKit

#endif // KOEKIT_WAVETABLES_BASIC_H
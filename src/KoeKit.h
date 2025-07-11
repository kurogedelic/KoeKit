#pragma once

/**
 * @file KoeKit.h
 * @brief High-performance audio synthesis library for RP2350A
 * 
 * KoeKit provides real-time audio synthesis optimized for RP2350A microcontrollers,
 * featuring modular oscillators, filters, and effects with FPU optimization.
 */

#ifndef KOEKIT_H
#define KOEKIT_H

#include <Arduino.h>
#include <array>
#include <cmath>
#include <algorithm>
#include <cstdint>

// Core configuration
#ifndef KOEKIT_SAMPLE_RATE
#define KOEKIT_SAMPLE_RATE 22050
#endif

#ifndef KOEKIT_WAVETABLE_SIZE
#define KOEKIT_WAVETABLE_SIZE 1024
#endif

// Include core modules
#include "core/wavetable_generator.h"
#include "wavetables/basic.h"
#include "core/oscillator.h"
#include "core/filter.h"
#include "core/envelope.h"
#include "core/audio_output.h"

/**
 * @namespace KoeKit
 * @brief Main namespace for all KoeKit functionality
 */
namespace KoeKit {
    constexpr uint32_t SAMPLE_RATE = KOEKIT_SAMPLE_RATE;
    constexpr size_t WAVETABLE_SIZE = KOEKIT_WAVETABLE_SIZE;
    constexpr float SAMPLE_RATE_F = static_cast<float>(SAMPLE_RATE);
    constexpr float TWO_PI = 6.28318530718f;
    
    /**
     * @brief Initialize KoeKit audio system
     * @param sample_rate Sample rate in Hz (default: 22050)
     * @param output_pin PWM output pin (default: 1)
     * @return true if initialization successful
     */
    bool begin(uint32_t sample_rate = SAMPLE_RATE, uint8_t output_pin = 1);
    
    /**
     * @brief Shutdown KoeKit audio system
     */
    void end();
    
    /**
     * @brief Get current sample rate
     * @return Sample rate in Hz
     */
    uint32_t getSampleRate();
}

#endif // KOEKIT_H
#pragma once

#include <stdint.h>

namespace Sft {

//! The default note16 returning the 0x1000 note
const uint16_t DEFAULT_NOTE16 = 0x4500;

//! The DSP frequency
const uint32_t DSP_FREQUENCY = 32000;

//! The DSP timer counter
const uint16_t DSP_TIMER_HZ = 8000;

//! The echo buffer size
const uint16_t MAX_ECHO_BUFFER_SIZE = 0x2800;

//! Maximum number of samples
const uint8_t MAX_NUM_SAMPLES = 32;

//! Maximum amount of channels
const uint8_t MAX_NUM_CHANNEL = 8;

//! Amount of notes per octave
const uint8_t NUM_NOTES_PER_OCTAVE = 12;

//! The minimum octave
const uint8_t FIRST_OCTAVE = 1;

//! The last octave
const uint8_t LAST_OCTAVE = 8;

//! The default octave
const uint8_t DEFAULT_OCTAVE = 7;

//! The amount of octaves in total
const uint8_t NUM_OCTAVES = LAST_OCTAVE - FIRST_OCTAVE + 1;

//! The initial volume
const uint8_t INITIAL_VOLUME = 0x3F;

//! The initial speed
const uint8_t INITIAL_SPEED = 0x60;

//! The duration of one measure
const uint8_t MEASURE_DURATION = 192;

//! Amount of ticks per measure
const uint8_t MEASURE_TICKS = 12;

//! Amount of subdivisions per tick
const uint8_t MEASURE_TICK_SUBDIVISION = 4;

//! The default sequence length
const uint32_t DEFAULT_SEQUENCE_TICKS = 16 * 4;

//! Precalculates constants
const uint32_t NUM_TICK_SUBTICKS = MEASURE_DURATION / MEASURE_TICKS;

}

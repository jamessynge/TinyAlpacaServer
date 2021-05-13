#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_INT_TYPES_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_INT_TYPES_H_

// The basic integer types, and an alternate approach to the min and max macros
// provided by Arduino.h.

#include <cstdint>

using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using int8_t = std::int8_t;
using int16_t = std::int16_t;
using int32_t = std::int32_t;

inline uint32_t min(uint32_t a, uint32_t b) { return (a <= b) ? a : b; }
inline uint32_t max(uint32_t a, uint32_t b) { return (a >= b) ? a : b; }

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_INT_TYPES_H_

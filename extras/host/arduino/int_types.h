#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_INT_TYPES_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_INT_TYPES_H_

// The basic integer types, and an alternate approach to the min and max macros
// provided by Arduino.h.

#include <stdint.h>  // pragma IWYU: export

inline uint32_t min(uint32_t a, uint32_t b) { return (a <= b) ? a : b; }
inline uint32_t max(uint32_t a, uint32_t b) { return (a >= b) ? a : b; }

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_INT_TYPES_H_

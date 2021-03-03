#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_ARDUINO_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_ARDUINO_H_

#include <cstdint>

#include "extras/host_arduino/pgmspace.h"  // IWYU pragma: export
#include "extras/host_arduino/print.h"     // IWYU pragma: export
#include "extras/host_arduino/serial.h"    // IWYU pragma: export

using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using int8_t = std::int8_t;
using int16_t = std::int16_t;
using int32_t = std::int32_t;

bool isPrintable(const char c);
bool isAlphaNumeric(const char c);
bool isUpperCase(const char c);

inline uint8_t max(uint8_t a, uint8_t b) { return (a >= b) ? a : b; }
inline uint32_t max(uint32_t a, uint32_t b) { return (a >= b) ? a : b; }

uint16_t millis();

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_ARDUINO_H_

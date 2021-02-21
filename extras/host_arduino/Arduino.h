#ifndef TINY_ALPACA_SERVER_ARDUINO_H_
#define TINY_ALPACA_SERVER_ARDUINO_H_

#include <cstdint>

#include "host_arduino/Print.h"  // IWYU pragma: export

namespace alpaca {

using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using int8_t = std::int8_t;
using int16_t = std::int16_t;
using int32_t = std::int32_t;

bool isPrintable(const char c);
bool isAlphaNumeric(const char c);
bool isUpperCase(const char c);

uint8_t max(uint8_t a, uint8_t b) { return (a >= b) ? a : b; }

// template <typename T,
//           typename E = typename std::enable_if<std::disjunction<
//               std::is_integral<T>, std::is_floating_point<T>>::value>::type>
// T max(T a, T b) {
//   return (a >= b) ? a : b;
// }

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_ARDUINO_H_

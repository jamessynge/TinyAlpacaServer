#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_H_

// Provides platform setup and exporting of platform specific header files.
//
// Why export all of these includes? Because otherwise lots of files have lots
// of includes with conditional compliation guards around them, which is pretty
// messy.
//
// Author: james.synge@gmail.com

#ifdef ARDUINO

#define TAS_EMBEDDED_TARGET 1
#define TAS_HOST_TARGET 0
#define TAS_ENABLE_DEBUGGING 0

#include <Arduino.h>  // IWYU pragma: export
#include <EEPROM.h>

#ifdef ARDUINO_ARCH_AVR
#include <avr/pgmspace.h>
#define AVR_PROGMEM PROGMEM
#else
#define AVR_PROGMEM
#endif  // ARDUINO_ARCH_AVR

#else  // !ARDUINO

#define TAS_EMBEDDED_TARGET 0
#define TAS_HOST_TARGET 1

#ifdef NDEBUG
#define TAS_ENABLE_DEBUGGING 0
#else
#define TAS_ENABLE_DEBUGGING 1
#endif  // NDEBUG

#include "extras/host/arduino/arduino.h"   // IWYU pragma: export
#include "extras/host/arduino/pgmspace.h"  // IWYU pragma: export
#include "extras/host/arduino/print.h"     // IWYU pragma: export
#include "extras/host/arduino/stream.h"    // IWYU pragma: export
#include "extras/host/eeprom/eeprom.h"     // IWYU pragma: export

#define AVR_PROGMEM

#endif  // ARDUINO

// If a function contains a TAS_DLOG, et al (e.g. when compiled for debugging),
// then the function can't be a constexpr. To allow for including these macros
// in such functions, we use these macros to choose whether it is a constexpr or
// not based on whether we've compiled it for debugging or not.
#if TAS_ENABLE_DEBUGGING
#define TAS_CONSTEXPR_FUNC
#define TAS_CONSTEXPR_VAR const
#else
#define TAS_CONSTEXPR_FUNC constexpr
#define TAS_CONSTEXPR_VAR constexpr
#endif  // TAS_ENABLE_DEBUGGING

// max is a macro in Arduino, but not on a host. We avoid 'confusion' by using
// inlineable function.
constexpr inline size_t MaxOf2(const size_t a, const size_t b) {
#ifdef ARDUINO
  return max(a, b);
#else   // !ARDUINO
  return (a >= b) ? a : b;
#endif  // ARDUINO
}
constexpr size_t MaxOf4(size_t a, size_t b, size_t c, size_t d) {
  return MaxOf2(MaxOf2(a, b), MaxOf2(c, d));
}

// The avr-gcc (C++11) compiler used by the Arduino IDE doesn't support
// __VA_OPT__, which is needed for the current definition of TAS_CHECK, etc.
// TODO(jamessynge): Come up with a way around that, including changing away
// from using variadic macros.

#if __cplusplus <= 201703 && defined __GNUC__ && !defined __clang__ && \
    !defined __EDG__  // Clang and EDG compilers may pretend to be GCC.
#define VA_OPT_SUPPORTED false
#else
#define VA_OPT_SUPPORTED_I2(a, b, c, ...) c
#define VA_OPT_SUPPORTED_I(...) \
  VA_OPT_SUPPORTED_I2(__VA_OPT__(, ), true, false, )
#define VA_OPT_SUPPORTED VA_OPT_SUPPORTED_I(?)
#endif

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_H_

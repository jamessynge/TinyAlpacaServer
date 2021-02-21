#ifndef TINY_ALPACA_SERVER_PLATFORM_H_
#define TINY_ALPACA_SERVER_PLATFORM_H_

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



#else  // !ARDUINO

#define TAS_EMBEDDED_TARGET 0
#define TAS_HOST_TARGET (!TAS_EMBEDDED_TARGET)

#ifdef NDEBUG
#define TAS_ENABLE_DEBUGGING 0
#else
#define TAS_ENABLE_DEBUGGING 1
#endif  // NDEBUG

#include "host_arduino/Arduino.h"  // IWYU pragma: export

#endif  // ARDUINO

// If a function contains a TAS_DLOG, et al, then the function can't be a
// constexpr. To allow for including these macros in such functions, we use
// these macros to choose whether it is a constexpr or not based on whether
// we've compiled it for debugging or not.
#if TAS_ENABLE_DEBUGGING
#define TAS_CONSTEXPR_FUNC
#define TAS_CONSTEXPR_VAR const
#else
#define TAS_CONSTEXPR_FUNC constexpr
#define TAS_CONSTEXPR_VAR constexpr
#endif  // TAS_ENABLE_DEBUGGING

constexpr size_t MaxOf2(size_t a, size_t b) { return max(a, b); }
constexpr size_t MaxOf4(
size_t a,
size_t b,
size_t c,
 size_t d) {
 return MaxOf2(MaxOf2(a, b), MaxOf2(c, d)); }



#endif  // TINY_ALPACA_SERVER_PLATFORM_H_

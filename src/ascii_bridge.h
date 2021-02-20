#ifndef TINY_ALPACA_SERVER_ASCII_BRIDGE_H_
#define TINY_ALPACA_SERVER_ASCII_BRIDGE_H_

// Provides common method names for some ASCII "character class" methods.
//
// Author: james.synge@gmail.com

#ifdef ARDUINO
#include <Arduino.h>
#else
#include "absl/strings/ascii.h"
#endif

namespace alpaca {

inline bool IsPrintable(const char c) {
#ifdef ARDUINO
  return isPrintable(c);
#else
  return absl::ascii_isprint(c);
#endif
}

inline bool IsAlphaNumeric(const char c) {
#ifdef ARDUINO
  return isAlphaNumeric(c);
#else
  return absl::ascii_isalnum(c);
#endif
}

inline bool IsUpperCase(const char c) {
#ifdef ARDUINO
  return isUpperCase(c);
#else
  return absl::ascii_isupper(c);
#endif
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_ASCII_BRIDGE_H_

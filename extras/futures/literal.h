#ifndef TINY_ALPACA_SERVER_EXTRAS_FUTURES_LITERAL_H_
#define TINY_ALPACA_SERVER_EXTRAS_FUTURES_LITERAL_H_

// This is an incomplete prototype of a Literal class, which references strings
// stored in PROGMEM (flash), avoiding the need to copy it into RAM. This is
// motivated by the Harvard Architecture of the AVR chips used in Arduinos,
// where program and data memory are in separate address spaces. Thus we can't
// directly access strings stored in RAM.

#include "src/platform.h"
#include "src/string_view.h"

// TO DO move includes and impls into .cc file.

#ifndef ARDUINO_ARCH_AVR
#include <cstring>
#endif  // ARDUINO_ARCH_AVR

namespace alpaca {

class Literal {
 public:
  // These two definitions must be changed together.
  using size_type = uint8_t;
  static constexpr size_type kMaxSize = 255;

  // Constructs from a string literal stored in AVR PROGMEM (or regular memory
  // on other CPU types).
  //
  // NOTE: The length of a C++ string literal includes the NUL (\0) at the end,
  // so we subtract one from N to get the length of the string before that.
  template <size_type N>
  constexpr Literal(const char (&buf)[N] PROGMEM)  // NOLINT
      : ptr_(buf), size_(N - 1) {}

  // Returns the number of characters in the StringView. Alias for length.
  constexpr size_type size() const { return size_; }

  // Returns true if the two strings are equal, with case sensitive comparison
  // of characters.
  bool operator==(const StringView& view) const {
    if (size_ != view.size()) {
      return false;
    }
#ifdef ARDUINO_ARCH_AVR
    return 0 == memcmp_P(ptr_, view.data(), size_);
#else   // !ARDUINO_ARCH_AVR
    return 0 == memcmp(ptr_, view.data(), size_);
#endif  // ARDUINO_ARCH_AVR
  }

  // Returns true if the two strings are equal, with case insensitive comparison
  // of characters. Neither string may have a NUL in the first 'size'
  // characters, else the comparison result may be wrong.
  bool case_equal(const StringView& view) const {
    if (size_ != view.size()) {
      return false;
    }
#ifdef ARDUINO_ARCH_AVR
    return 0 == strncmp_P(ptr_, view.data(), size_);
#else   // !ARDUINO_ARCH_AVR
    return 0 == strncmp(ptr_, view.data(), size_);
#endif  // ARDUINO_ARCH_AVR
  }

  // Copy the string (nul-terminated) to the provided output buffer, which is of
  // size 'size'. If 'size' is large enough (size_+1), then the string is copied
  // and true is returned; if size is less than (size_+1), then false is
  // returned and no copying is performed.
  bool copyTo(char* out, size_type size) {
    if (size <= size_) {
      return false;
    }
#ifdef ARDUINO_ARCH_AVR
    memcpy_P(out, ptr_, size_ + 1);
#else   // !ARDUINO_ARCH_AVR
    memcpy(out, ptr_, size_ + 1);
#endif  // ARDUINO_ARCH_AVR
    return true;
  }

  // Print (write) the string to the provided Print instance.
  size_t printTo(Print& out) const;

  // Print (write) the string to the provided Print instance, surrounded by
  // double quotes and with escaping as required by JSON.
  size_t printJsonEscapedTo(Print& out) const;

 private:
  PGM_P const ptr_;
  size_type size_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_FUTURES_LITERAL_H_

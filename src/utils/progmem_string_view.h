#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PROGMEM_STRING_VIEW_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PROGMEM_STRING_VIEW_H_

// ProgmemStringView is like StringView, but for a view of a string stored in
// program memory (aka PROGMEM), rather than in read-write memory (aka RAM). On
// microcontrollers, this typically means flash memory.
//
// But why do we need a separate type for a view on a read-only string? Because
// AVR microcontrollers have multiple address spaces, meaning that we can't tell
// from the value of an address whether it is in RAM or Flash, or EEPROM for
// that matter; we need to know also which space it is in. This multiple address
// spaces design is known as Harvard Architecture.
//
// So, ProgmemStringView has support for using alternate instructions (via AVR
// Libc's progmem library) to access tye characters in a string.
//
// NOTE: So far I've written this using PGM_P pointers, which, IIUC, are limited
// to the first 64KB of flash. I don't know what guarantees there are about the
// placement of variables marked PROGMEM, in particular whether there is an
// attempt to place them early in the address space.
//
// Author: james.synge@gmail.com

#include "utils/platform.h"

namespace alpaca {

class ProgmemStringView {
 public:
  // These two definitions must be changed together.
  using size_type = uint8_t;
  static constexpr size_type kMaxSize = 255;

  // Construct empty.
  constexpr ProgmemStringView() noexcept : ptr_(nullptr), size_(0) {}

  constexpr ProgmemStringView(PGM_P ptr, size_type length)
      : ptr_(ptr), size_(length) {}

  // Constructs from a string literal stored in AVR PROGMEM (or regular memory
  // on other CPU types).
  //
  // NOTE: The length of a C++ string literal includes the NUL (\0) at the end,
  // so we subtract one from N to get the length of the string before that.
  template <size_type N>
  explicit constexpr ProgmemStringView(const char (&buf)[N] PROGMEM)
      : ptr_(buf), size_(N - 1) {}

  // Copy constructor and assignment operator.
  constexpr ProgmemStringView(const ProgmemStringView&) = default;
  ProgmemStringView& operator=(const ProgmemStringView&) = default;

  // Print the string to the provided Print instance.
  size_t printTo(Print& out) const;

  // Returns true if the other literal has the same value.
  bool operator==(const ProgmemStringView& other) const;

  // Returns true if the other literal has a different value.
  bool operator!=(const ProgmemStringView& other) const {
    return !(*this == other);
  }

  // Returns true if the two instances are identical views of the same PROGMEM
  // string. This is really useful just for testing that the compiler and linker
  // are combining strings together as expected.
  bool Identical(const ProgmemStringView& other) const;

  // Returns true if the two strings are equal, with case sensitive comparison
  // of characters. other points to a string in RAM, not PROGMEM.
  bool Equal(const char* other, size_type other_size) const;

  // Returns true if the two strings are equal, with case insensitive comparison
  // of characters. other points to a string in RAM, not PROGMEM.
  bool CaseEqual(const char* other, size_type other_size) const;

  // Returns true if the two strings are equal, after lower-casing this PROGMEM
  // string. other points to a string in RAM, not PROGMEM.
  bool LoweredEqual(const char* other, size_type other_size) const;

  // Returns true if the other string starts with this literal string.
  bool IsPrefixOf(const char* other, size_type other_size) const;

  // If 'size_' is not greater than the provided 'size', copies the literal
  // string into *out. No NUL terminator is copied.
  bool CopyTo(char* out, size_type size);

  // Support for iterating.
  constexpr PGM_P begin() const { return ptr_; }
  constexpr PGM_P end() const { return ptr_ + size_; }

  // Returns the character the the specified position ([0..size_)) within the
  // string.
  char at(size_type pos) const;

  // Returns the number of characters in the string.
  constexpr size_type size() const { return size_; }

 private:
  PGM_P ptr_;
  size_type size_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PROGMEM_STRING_VIEW_H_

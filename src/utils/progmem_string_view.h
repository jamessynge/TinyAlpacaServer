#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PROGMEM_STRING_VIEW_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PROGMEM_STRING_VIEW_H_

// ProgmemStringView is like StringView, but for a string stored in PROGMEM
// rather than in RAM. This avoids the need to copy it into RAM, saving lots of
// RAM vs. strings that aren't (somehow) annotated for storage only in PROGMEM.
// This is motivated by the Harvard Architecture of the AVR chips used in
// Arduinos, where program and data memory (i.e. PROGMEM and RAM) are in
// separate address spaces. Thus we can't directly access strings stored in
// PROGMEM.
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
  PGM_P begin() const { return ptr_; }
  PGM_P end() const { return ptr_ + size_; }

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

#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LITERAL_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LITERAL_H_

// This is an incomplete prototype of a Literal class, which references strings
// stored in PROGMEM (flash), avoiding the need to copy it into RAM. This is
// motivated by the Harvard Architecture of the AVR chips used in Arduinos,
// where program and data memory are in separate address spaces. Thus we can't
// directly access strings stored in RAM.
//
// NOTE: So far I've written this using PGM_P pointers, which, IIUC, are limited
// to the first 64KB of flash. I don't know what guarantees there are about the
// placement of variables marked PROGMEM, in particular whether there is an
// attempt to place them early in the address space.

#if TAS_HOST_TARGET
#include <ostream>
#include <string>
#endif  // TAS_HOST_TARGET

#include "utils/platform.h"

namespace alpaca {

// We include the TAS_CONSTEXPR_FUNC specifier only on constructors, as those
// are the only one's we're trying to guarantee are executable at compile time.
// However, if we want to be able to create Literals from substrings (e.g.
// "action" is a substring of "supportedactions"), then we could add a constexpr
// substr and a constructor that accepts a constexpr Literal.

class Literal {
 public:
  // These two definitions must be changed together.
  using size_type = uint8_t;
  static constexpr size_type kMaxSize = 255;

  // Construct empty.
  TAS_CONSTEXPR_FUNC Literal() noexcept : ptr_(nullptr), size_(0) {}

  // Constructs from a string literal stored in AVR PROGMEM (or regular memory
  // on other CPU types).
  //
  // NOTE: The length of a C++ string literal includes the NUL (\0) at the end,
  // so we subtract one from N to get the length of the string before that.
  template <size_type N>
  explicit constexpr Literal(const char (&buf)[N] PROGMEM)
      : ptr_(buf), size_(N - 1) {}

  // Construct with a specified length. This supports storing multiple Literals
  // in a single string literal (e.g. 3 strings concatenated together, such as
  // "Token1MessageAToken2").
  TAS_CONSTEXPR_FUNC Literal(PGM_P ptr, size_type length)
      : ptr_(ptr), size_(length) {}

  // Copy constructor and assignment operator.
  TAS_CONSTEXPR_FUNC Literal(const Literal&) = default;
  Literal& operator=(const Literal&) = default;

  // Returns true if the other literal has the same value.
  bool operator==(const Literal& other) const;

  // Returns true if the other literal pointers to the same string literal.
  bool same(const Literal& other) const;

  // Returns the number of characters in the string.
  size_type size() const { return size_; }

  // Returns the character the the specified position ([0..size_)) within the
  // string.
  char at(size_type pos) const;

  // Returns true if the two strings are equal, with case sensitive comparison
  // of characters.
  bool equal(const char* other, size_type other_size) const;

  // Returns true if the two strings are equal, with case insensitive comparison
  // of characters.
  bool case_equal(const char* other, size_type other_size) const;

  // Returns true if the other string starts with this literal string.
  bool is_prefix_of(const char* other, size_type other_size) const;

  // If 'size_' is not greater than the provided 'size', copies the literal
  // string into *out. No NUL terminator is copied.
  bool copyTo(char* out, size_type size);

  // Print the string to the provided Print instance. This is not a virtual
  // function (i.e. not an override of Printable::printTo) because that would
  // remove the ability for this to have a constexpr ctor in C++ < 20.
  size_t printTo(Print& out) const;

  // In support of tests, returns the address in PROGMEM of the string.
  // On a typical (Von Neumann) host, this is in the same address space as data.
  PGM_VOID_P prog_data_for_tests() const { return ptr_; }

#if TAS_HOST_TARGET
  friend std::ostream& operator<<(std::ostream& out, const Literal& literal);
#endif

 private:
  PGM_P ptr_;
  size_type size_;
};

// This is an array of Literal instances, stored in RAM, not PROGMEM.
struct LiteralArray {
  constexpr LiteralArray() : array(nullptr), size(0) {}
  template <size_t N>
  explicit constexpr LiteralArray(const Literal (&literal_strings)[N])
      : array(literal_strings), size(N) {}

  const Literal* begin() const { return array; }
  const Literal* end() const { return array + size; }

  const Literal* array;
  const size_t size;
};

#if TAS_HOST_TARGET
// Returns a std::string with the value of the view.
std::string ToStdString(const Literal& literal);

// Returns a quoted and hex escaped string from the characters in the view.
std::string ToHexEscapedString(const Literal& literal);
#endif  // TAS_HOST_TARGET

}  // namespace alpaca

// Can place a TAS_DEFINE_LITERAL in any source file, where it will create a
// constexpr 'variable' and a function creating a Literal based on that
// variable.
#ifndef TAS_DEFINE_LITERAL

#define TAS_DEFINE_LITERAL(name, literal)             \
  constexpr char kLiteral_##name[] PROGMEM = literal; \
  inline constexpr ::alpaca::Literal name() {         \
    return ::alpaca::Literal(kLiteral_##name);        \
  }

#endif  // !TAS_DEFINE_LITERAL

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LITERAL_H_

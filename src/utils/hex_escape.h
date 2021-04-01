#ifndef TINY_ALPACA_SERVER_SRC_UTILS_HEX_ESCAPE_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_HEX_ESCAPE_H_

// Support for printing strings (Printable's or similar) with non-printable
// ASCII characters hex escaped. Intended to produce output that is valid as a
// C/C++ string literal.

#include "utils/platform.h"

namespace alpaca {

// Print |c| hex escaped to |out|. Actually backslash escapes backslash and
// double quote, and uses named escapes for newline (\n), carriage return (\t),
// etc.; for other control characters and non-ascii characters, uses a hex
// escape (e.g. \x01 or \xff).
size_t PrintCharHexEscaped(Print& out, const char c);

// Wraps a Print instance, forwards output to that instance with hex escaping
// applied. Note that this does NOT add double quotes before and after the
// output.
class PrintHexEscaped : public Print {
 public:
  explicit PrintHexEscaped(Print& wrapped);

  // These are the two abstract virtual methods in Arduino's Print class. I'm
  // presuming that the uint8_t 'b' is actually an ASCII char.
  size_t write(uint8_t b) override;
  size_t write(const uint8_t* buffer, size_t size) override;

  // Export the other write methods.
  using Print::write;

 private:
  Print& wrapped_;
};

template <class T>
class HexEscapedPrintable : public Printable {
 public:
  explicit HexEscapedPrintable(const T& wrapped) : wrapped_(wrapped) {}

  size_t printTo(Print& raw_out) const override {
    size_t count = raw_out.print('"');
    PrintHexEscaped out(raw_out);
    count += wrapped_.printTo(out);
    count += raw_out.print('"');
    return count;
  }

 private:
  const T& wrapped_;
};

template <typename T>
inline HexEscapedPrintable<T> HexEscaped(const T& like_printable) {
  return HexEscapedPrintable<T>(like_printable);
}

// template <typename T>
// class HexEncodedInteger : public Printable {
//  public:
//   explicit HexEncodedInteger(T value) : value_(value) {}

//   size_t printTo(Print& out) const override { out.print(value_, 16); }

//  private:
//   const T value_;
// };

// template <>
// inline HexEscapedPrintable HexEscaped(const Printable& printable) {
//   return HexEscapedPrintable(printable);
// }

// inline HexEncodedInteger HexEncoded(uint8_t value) {
//   return HexEncodedInteger(value);
// }

// inline HexEncodedInteger HexEncoded(int8_t value) {
//   return HexEncodedInteger(static_cast<uint32_t>(value));
// }

// inline HexEncodedInteger HexEncoded(uint16_t value) {
//   return HexEncodedInteger(value);
// }

// inline HexEncodedInteger HexEncoded(int16_t value) {
//   return HexEncodedInteger(static_cast<uint32_t>(value));
// }

// inline HexEncodedInteger HexEncoded(uint32_t value) {
//   return HexEncodedInteger(value);
// }

// inline HexEncodedInteger HexEncoded(int32_t value) {
//   return HexEncodedInteger(static_cast<uint32_t>(value));
// }

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_HEX_ESCAPE_H_

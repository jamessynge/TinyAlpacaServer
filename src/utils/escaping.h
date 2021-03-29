#ifndef TINY_ALPACA_SERVER_SRC_UTILS_ESCAPING_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_ESCAPING_H_

// Core of support for printing JSON strings. Characters that are not valid in
// JSON strings (e.g. Ctrl-A) are not printed.

#include "utils/platform.h"

namespace alpaca {

size_t PrintCharHexEscaped(Print& out, const char c);

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

class HexEscapedPrintable : public Printable {
 public:
  explicit HexEscapedPrintable(const Printable& wrapped);

  size_t printTo(Print& out) const override;

 private:
  const Printable& wrapped_;
};

class HexEscapedInteger : public Printable {
 public:
  explicit HexEscapedInteger(uint32_t value) : value_(value) {}

  size_t printTo(Print& out) const override;

 private:
  const uint32_t value_;
};

namespace escaping_internal {

template <class T>
class HexEscapedLikePrintable : public Printable {
 public:
  explicit HexEscapedLikePrintable(const T& wrapped) : wrapped_(wrapped){}

  size_t printTo(Print& out) const {
  size_t count = raw_out.print('"');
  PrintHexEscaped out(raw_out);
  count += wrapped_.printTo(out);
  count += raw_out.print('"');
  return count;
}

 private:
  const Printable& wrapped_;
};
}  // namespace escaping_internal

template <typename T>
inline HexEscapedPrintable HexEscaped(const T& like_printable) {
  return escaping_internal::HexEscaped

      // Selects the specialization of OPrintStream::print based on whether T
      // has a printTo(Print&) member function.
      OPrintStream::print(out.out_, value, has_print_to<T>{});

  return HexEscapedPrintable(printable);
}

template <>
inline HexEscapedPrintable HexEscaped(const Printable& printable) {
  return HexEscapedPrintable(printable);
}

template <>
inline HexEscapedInteger HexEscaped(uint8_t value) {
  return HexEscapedInteger(value);
}

template <>
inline HexEscapedInteger HexEscaped(int8_t value) {
  return HexEscapedInteger(static_cast<uint32_t>(value));
}

template <>
inline HexEscapedInteger HexEscaped(uint16_t value) {
  return HexEscapedInteger(value);
}

template <>
inline HexEscapedInteger HexEscaped(int16_t value) {
  return HexEscapedInteger(static_cast<uint32_t>(value));
}

template <>
inline HexEscapedInteger HexEscaped(uint32_t value) {
  return HexEscapedInteger(value);
}

template <>
inline HexEscapedInteger HexEscaped(int32_t value) {
  return HexEscapedInteger(static_cast<uint32_t>(value));
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_ESCAPING_H_

#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PRINTABLE_PROGMEM_STRING_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PRINTABLE_PROGMEM_STRING_H_

// PrintableProgmemString wraps a pointer to a string literal of known size
// stored in PROGMEM, and makes it printable.

#include "utils/platform.h"

namespace alpaca {

class PrintableProgmemString : public Printable {
 public:
  // These two definitions must be changed together.
  using size_type = uint8_t;
  static constexpr size_type kMaxSize = 255;

  // Construct empty.
  PrintableProgmemString() noexcept : ptr_(nullptr), size_(0) {}

  PrintableProgmemString(PGM_P ptr, size_type length)
      : ptr_(ptr), size_(length) {}

  // Print the string to the provided Print instance.
  size_t printTo(Print& out) const override;

  // Returns the number of characters in the string.
  constexpr size_type size() const { return size_; }

  // On a typical (Von Neumann) host, this is in the same address space as data.
  PGM_VOID_P progmem_data() const { return ptr_; }

 private:
  PGM_P ptr_;
  size_type size_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PRINTABLE_PROGMEM_STRING_H_

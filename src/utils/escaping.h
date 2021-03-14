#ifndef TINY_ALPACA_SERVER_SRC_UTILS_ESCAPING_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_ESCAPING_H_

// Core of support for printing JSON strings. Characters that are not valid in
// JSON strings (e.g. Ctrl-A) are not printed.

#include "utils/platform.h"

#if TAS_HOST_TARGET
#include <ostream>
#endif  // TAS_HOST_TARGET

namespace alpaca {

// Wraps a Print instance, forwards output to that instance with JSON escaping
// applied. Note that this does NOT add double quotes before and after the
// output.
class PrintJsonEscaped : public Print {
 public:
  explicit PrintJsonEscaped(Print& wrapped);

  // These are the two abstract virtual methods in Arduino's Print class. I'm
  // presuming that the uint8_t 'b' is actually an ASCII char.
  size_t write(uint8_t b) override;
  size_t write(const uint8_t* buffer, size_t size) override;

  // Export the other write methods.
  using Print::write;

 private:
  Print& wrapped_;
};

// Print 'value' to 'raw_output', with the characters JSON escaped. Note that
// this does NOT add double quotes before and after the output.
size_t PrintJsonEscapedTo(const Printable& value, Print& raw_output);

// Print 'value' to 'raw_output' as a JSON string, i.e. starting and ending with
// double quotes, and with the characters printed by 'value' JSON escaped.
size_t PrintJsonEscapedStringTo(const Printable& value, Print& raw_output);

// Print c with appropriate escaping for JSON.
size_t PrintCharJsonEscaped(Print& out, char c);

#if TAS_HOST_TARGET
// Insert the escaped character into the ostream. Supports streaming
// JsonStringView instances, useful for logging and debugging.
void StreamCharJsonEscaped(std::ostream& out, const char c);
#endif  // TAS_HOST_TARGET

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_ESCAPING_H_

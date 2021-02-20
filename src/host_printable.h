#ifndef TINY_ALPACA_SERVER_HOST_PRINTABLE_H_
#define TINY_ALPACA_SERVER_HOST_PRINTABLE_H_

// When compiled for the host, as opposed to for Arduino, we need our own
// minimal implementation of the Print class, and the definition of the
// Printable interface.

#include <stddef.h>
#include <stdint.h>

#include <ostream>
#include <string>
#include <type_traits>

#include "config.h"

namespace alpaca {

#if ALPACA_SERVER_ON_HOST

class Print;

class Printable {
 public:
  virtual ~Printable() {}
  virtual size_t printTo(Print& p) const = 0;
};

// This is just enough to support our needs, no more... I hope.
class Print {
 public:
  Print() : out_(nullptr) {}
  explicit Print(std::ostream& out) : out_(&out) {}
  virtual ~Print() {}

  virtual size_t write(uint8_t b) {
    return write(reinterpret_cast<char*>(&b), 1);
  }
  virtual size_t write(const char* buffer, size_t count) {
    if (out_) {
      out_->write(buffer, count);
    }
    return count;
  }

  template <typename T,
            typename E = typename std::enable_if<std::disjunction<
                std::is_integral<T>, std::is_floating_point<T>>::value>::type>
  size_t print(T value) {
    auto s = std::to_string(value);
    return write(s.data(), s.size());
  }

  size_t print(const Printable& value) { return value.printTo(*this); }

 private:
  std::ostream* const out_;
};
#endif  // ALPACA_SERVER_ON_HOST

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_HOST_PRINTABLE_H_

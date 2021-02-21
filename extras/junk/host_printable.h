#ifndef TINY_ALPACA_SERVER_HOST_PRINTABLE_H_
#define TINY_ALPACA_SERVER_HOST_PRINTABLE_H_

// When compiled for the host, as opposed to for Arduino, we need our own
// minimal implementation of the Print class, and the definition of the
// Printable interface.
//
// Author: james.synge@gmail.com

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

  // These are the two abstract virtual methods in Arduino's Print class.
  virtual size_t write(uint8_t b) {
    const uint8_t* buffer = &b;
    return write(buffer, 1);
  }
  virtual size_t write(const uint8_t* buffer, size_t size) {
    if (out_) {
      out_->write(reinterpret_cast<const char*>(buffer), size);
    }
    return size;
  }

  // These are the specializations they provide for handling chars.
  size_t write(const char* str) {
    if (str == nullptr) return 0;
    return write(reinterpret_cast<const uint8_t*>(str), strlen(str));
  }
  size_t write(const char* buffer, size_t size) {
    return write(reinterpret_cast<const uint8_t*>(buffer), size);
  }

  // Print an integer or floating point. For simplicity, we just use
  // std::to_string to perform the conversion to text.
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

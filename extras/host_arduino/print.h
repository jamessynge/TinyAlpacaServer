#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PRINT_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PRINT_H_

// When compiled for the host, as opposed to for Arduino, we need our own
// minimal implementation of the Print class, and the Printable interface.
//
// Author: james.synge@gmail.com

#include <cstddef>
#include <cstdint>
#include <ostream>

namespace alpaca {

class Print;

class Printable {
 public:
  virtual ~Printable();
  virtual size_t printTo(Print& p) const = 0;

  // Support for tests and logging.
  friend std::ostream& operator<<(std::ostream& out,
                                  const Printable& printable);
};

// This is just enough to support our needs, no more... I hope.
class Print {
 public:
  Print();
  virtual ~Print();

  // These are the two abstract virtual methods in Arduino's Print class.
  virtual size_t write(uint8_t b) = 0;
  virtual size_t write(const uint8_t* buffer, size_t size) = 0;

  // These are the specializations they provide for handling chars.
  size_t write(const char* str);
  size_t write(const char* buffer, size_t size);

  size_t print(char c);
  size_t print(uint8_t value);
  size_t print(int16_t value);
  size_t print(uint16_t value);
  size_t print(int32_t value);
  size_t print(uint32_t value);
  size_t print(double value);
  size_t print(const Printable& value);

 private:
  size_t printInteger(int64_t value);
  size_t printDouble(double value);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PRINT_H_

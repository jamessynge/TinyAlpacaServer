#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PRINT_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PRINT_H_

// When compiled for the host, as opposed to for Arduino, we need our own
// minimal implementation of the Print class, and the Printable interface.
//
// Author: james.synge@gmail.com

#include <cstddef>
#include <cstdint>
#include <ostream>

class Print;

class Printable {
 public:
  virtual ~Printable();
  virtual size_t printTo(Print& p) const = 0;

  // Support for tests and logging.
  friend std::ostream& operator<<(std::ostream& out,
                                  const Printable& printable);
};

#define DEC 10
#define HEX 16

// This is just enough to support our needs, no more... I hope.
class Print {
 public:
  Print();
  virtual ~Print();

  // These are the two abstract virtual methods in Arduino's Print class.

  // Writes 'b', one byte, to the output, returns the number of bytes (0 or 1)
  // actually written (i.e. 0 if something has gone wrong, else 1).
  virtual size_t write(uint8_t b) = 0;

  // Writes the 'size' bytes starting at *buffer to the output, returns the
  // number of bytes written.
  virtual size_t write(const uint8_t* buffer, size_t size) = 0;

  // Writes the NUL terminated string 'str' to the output, returns the number of
  // bytes written.
  size_t write(const char* str);

  // Writes the 'size' characters starting at *buffer to the output, returns the
  // number of bytes written.
  size_t write(const char* buffer, size_t size);

  // Prints (writes) a value of various types. 'short' isn't supported on
  // Arduino AVR, where int == uint16_t (i.e. int and short are the same type on
  // that platform). To make it testable on host, we support short explicitly.
  size_t print(const char str[]);
  size_t print(char c);
  size_t print(unsigned char value, int base = DEC);
  size_t print(short value, int base = DEC);           // NOLINT
  size_t print(unsigned short value, int base = DEC);  // NOLINT
  size_t print(int value, int base = DEC);
  size_t print(unsigned int value, int base = DEC);
  size_t print(long value, int base = DEC);           // NOLINT
  size_t print(unsigned long value, int base = DEC);  // NOLINT
  size_t print(double value, int digits = 2);
  size_t print(const Printable& value);

  // Prints (writes) a value of various types, with a trailing \n.
  size_t println(const char str[]);
  size_t println(char c);
  size_t println(unsigned char value, int base = DEC);
  size_t println(int value, int base = DEC);
  size_t println(unsigned int value, int base = DEC);
  size_t println(short value, int base = DEC);           // NOLINT
  size_t println(unsigned short value, int base = DEC);  // NOLINT
  size_t println(long value, int base = DEC);            // NOLINT
  size_t println(unsigned long value, int base = DEC);   // NOLINT
  size_t println(double value, int digits = 2);
  size_t println(const Printable& value);
  size_t println();

  virtual void flush();

 private:
  size_t printDouble(double value, int digits);
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PRINT_H_

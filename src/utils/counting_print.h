#ifndef TINY_ALPACA_SERVER_SRC_UTILS_COUNTING_PRINT_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_COUNTING_PRINT_H_

// CountingPrint extends Print with counting of the number of bytes printed.
//
// PrintNoOp extends Print, but drops the bytes printed on the floor.
//
// Combining the two allows us to compute the value to be placed in the
// Content-Length header of an HTTP response message.
//
// Author: james.synge@gmail.com

#include "mcucore_platform.h"

namespace alpaca {

class PrintNoOp : public Print {
 public:
  // These are the two abstract virtual methods in Arduino's Print class.
  size_t write(uint8_t) override { return 1; }
  size_t write(const uint8_t* buffer, size_t size) override { return size; }

  // Pull in the other variants of write; otherwise, only the above two are
  // visible.
  using Print::write;
};

class CountingPrint : public Print {
 public:
  explicit CountingPrint(Print& out) : out_(out), count_(0) {}

  // These are the two abstract virtual methods in Arduino's Print class.
  size_t write(uint8_t value) override;
  size_t write(const uint8_t* buffer, size_t size) override;

  // Pull in the other variants of write; otherwise, only the above two are
  // visible.
  using Print::write;

  // The total count of bytes written.
  uint32_t count() const { return count_; }

 private:
  Print& out_;
  uint32_t count_;
};

uint32_t SizeOfPrintable(const Printable& value);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_COUNTING_PRINT_H_

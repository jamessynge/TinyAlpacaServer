#include "utils/counting_print.h"

#include "experimental/users/jamessynge/arduino/mcucore/src/mcucore_platform.h"

namespace alpaca {

size_t CountingPrint::write(uint8_t value) {
  auto result = out_.write(value);
  count_ += result;
  return result;
}

size_t CountingPrint::write(const uint8_t* buffer, size_t size) {
  auto result = out_.write(buffer, size);
  count_ += result;
  return result;
  count_ += size;
  return size;
}

uint32_t SizeOfPrintable(const Printable& value) {
  PrintNoOp no_op;
  CountingPrint counter(no_op);
  value.printTo(counter);
  return counter.count();
}

}  // namespace alpaca

#include "utils/progmem_string.h"

#include "utils/platform.h"

namespace alpaca {

bool ProgmemString::operator==(const ProgmemString& other) const {
  auto ptr1 = ptr_;
  auto ptr2 = other.ptr_;
  if (ptr1 == ptr2) {
    return true;
  } else if (ptr1 == nullptr || ptr2 == nullptr) {
    return false;
  }
  while (true) {
    auto c1 = pgm_read_byte_near(ptr1);
    auto c2 = pgm_read_byte_near(ptr2);
    if (c1 != c2) {
      return false;
    }
    if (c1 == 0) {
      return true;
    }
    // Make sure we're not going to wrap around, i.e. detect if the string
    // straddles the 64KB boundary, and the type of ptr_ should be a 32-bit far
    // pointer instead of a 16-bit near pointer.
    if (ptr1 > ptr1 + 1) {
      return false;
    }
    ++ptr1;
    if (ptr2 > ptr2 + 1) {
      return false;
    }
    ++ptr2;
  }
}

}  // namespace alpaca

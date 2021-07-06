#ifndef TINY_ALPACA_SERVER_SRC_UTILS_TINY_STRING_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_TINY_STRING_H_

// TinyString is a string with a fixed maximum size and no NUL termination.
// It can be used where we know the maximum size needed for a small, variable
// length string; for example, storing the values of some parameters that might
// come in requests.
//
// Author: james.synge@gmail.com

#include "utils/logging.h"
#include "utils/platform.h"

namespace alpaca {

// A very, very small string class with an embedded char array and size.
template <uint8_t N>
class TinyString {
 public:
  using size_type = uint8_t;

  void Clear() { size_ = 0; }

  // Set the string by copying the 'size' characters from 'from'.
  // Returns false if size is too large.
  bool Set(const char* from, size_type size) {
    TAS_DCHECK_LE(size, N) << TAS_FLASHSTR("Too big");
    Clear();
    if (size > N) {
      return false;
    }
    memcpy(data_, from, size);
    size_ = size;
    return true;
  }

  const char* data() const { return data_; }
  char* data() { return data_; }

  size_type size() const { return size_; }
  static constexpr size_type maximum_size() { return N; }

  // Set the size explicitly. This allows for data to be copied into here from
  // PROGMEM by a caller, without having to add PROGMEM support here.
  // Returns false if size is too large.
  bool set_size(size_type size) {
    TAS_DCHECK_LE(size, N) << TAS_FLASHSTR("Too big");
    if (size > N) {
      return false;
    }
    size_ = size;
    return true;
  }

 private:
  uint8_t size_{0};
  char data_[N];
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_TINY_STRING_H_

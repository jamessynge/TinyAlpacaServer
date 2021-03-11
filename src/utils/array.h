#ifndef TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_H_

// Array is similar to std::array and std::initializer_list, wraps a C++ array
// of T (or const T) of known (fixed) size, and provides support for iterating
// through the array. It exists to make it easier to iterate through static
// arrays provided to TinyAlpacaServer as its configuration data.

#include "utils/platform.h"

namespace alpaca {

template <typename T>
class Array {
 public:
  // These two definitions must be changed together.
  using size_type = uint8_t;
  static constexpr size_type kMaxSize = 255;

  using value_type = T;
  using reference = value_type&;
  using const_iterator = const value_type*;
  using iterator = value_type*;

  // Construct empty.
  constexpr Array() noexcept : ptr_(nullptr), size_(0) {}

  // Construct with a specified length.
  constexpr Array(T* ptr, size_type length)
      : ptr_(length > 0 ? ptr : nullptr), size_(length) {}

  // Constructs from a literal T[] (i.e. array of T). The goal of this is to get
  // the compiler to populate the size, rather than computing it at runtime.
  template <size_type N>
  explicit Array(T (&array)[N]) : Array(array, N) {}

  iterator begin() { return ptr_; }
  const_iterator begin() const { return ptr_; }
  iterator end() { return ptr_ + size_; }
  const_iterator end() const { return ptr_ + size_; }

  // Returns the number of elements in the array.
  constexpr size_type size() const { return size_; }

  // Returns a pointer to the first element of the underlying array.
  constexpr const T* data() const { return ptr_; }

 private:
  T* ptr_;
  size_type size_;
};

template <typename T, int N>
Array<T> MakeArray(T (&array)[N]) {
  return Array<T>(array, N);
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_H_

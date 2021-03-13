#ifndef TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_VIEW_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_VIEW_H_

// ArrayView references a C-style array of T (or const T) of known (fixed) size.
// It provides STL style support for iterating over the C-style array, in
// support of dealing with static (maybe const) configuration data provided to
// TinyAlpacaServer.

#include "utils/platform.h"

namespace alpaca {

template <typename T>
class ArrayView {
 public:
  // These two definitions must be changed together.
  using size_type = uint8_t;
  static constexpr size_type kMaxSize = 255;

  using value_type = T;
  using reference = value_type&;
  using const_iterator = const value_type*;
  using iterator = value_type*;

  // Construct empty.
  constexpr ArrayView() noexcept : ptr_(nullptr), size_(0) {}

  // Construct with a specified length.
  constexpr ArrayView(T* ptr, size_type length)
      : ptr_(length > 0 ? ptr : nullptr), size_(length) {}

  // Constructs from a literal T[] (i.e. array of T). The goal of this is to get
  // the compiler to populate the size, rather than computing it at runtime.
  template <size_type N>
  explicit ArrayView(T (&array)[N]) : ArrayView(array, N) {}

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
ArrayView<T> MakeArrayView(T (&array)[N]) {
  return ArrayView<T>(array, N);
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_VIEW_H_

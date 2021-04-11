#ifndef TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_VIEW_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_VIEW_H_

// ArrayView references a C-style array of T of known (fixed) size. It provides
// STL style support for iterating over the C-style array, in support of dealing
// with static configuration data provided to TinyAlpacaServer.

#include "utils/logging.h"
#include "utils/platform.h"
#include "utils/traits/type_traits.h"

namespace alpaca {

template <typename T>
class ArrayView {
 public:
  // These two definitions must be changed together.
  using size_type = uint8_t;
  static constexpr size_type kMaxSize = 255;

  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = value_type*;
  using const_iterator = const value_type*;

  // Construct empty.
  constexpr ArrayView() noexcept : ptr_(nullptr), size_(0) {}

  // Construct with a specified length.
  constexpr ArrayView(T* ptr, size_type length)
      : ptr_(length > 0 ? ptr : nullptr), size_(length) {}

  // Constructs from a literal T[N] (i.e. an array of N elements of type T). The
  // goal of this is to get the compiler to populate the size, rather than
  // computing it at runtime.
  template <size_type N>
  explicit ArrayView(T (&array)[N]) : ArrayView(array, N) {}

  const_iterator begin() { return ptr_; }
  const_iterator begin() const { return ptr_; }
  const_iterator end() { return ptr_ + size_; }
  const_iterator end() const { return ptr_ + size_; }

  // Returns the number of elements in the array.
  constexpr size_type size() const { return size_; }

  // Returns a pointer to the first element of the underlying array.
  constexpr T* data() const { return ptr_; }

  // Element access:
  reference operator[](size_type ndx) {
    TAS_DCHECK_LT(ndx, size_);
    return ptr_[ndx];
  }
  const_reference operator[](size_type ndx) const {
    TAS_DCHECK_LT(ndx, size_);
    return ptr_[ndx];
  }

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

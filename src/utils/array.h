#ifndef TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_H_

// Array is similar to std::array; it contains (owns) a C-style array of T of
// known (fixed) size, and provides support for iterating through the array. A
// key application is supporting PrintableCat / AnyPrintableArray.
//
// Author: james.synge@gmail.com

#include "logging.h"
#include "mcucore_platform.h"
#include "utils/array_view.h"

namespace alpaca {

template <typename T, size_t SIZE>
struct Array {
 public:
  using value_type = T;
  using size_type = size_t;
#ifndef ARDUINO_ARCH_AVR
  using difference_type = ptrdiff_t;
#endif  // !ARDUINO_ARCH_AVR
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = value_type*;
  using const_iterator = const value_type*;

  // Copy from the provided array.
  template <typename U, size_t N>
  void copy(const U (&data)[N]) {
    size_type limit = N > SIZE ? SIZE : N;
    for (size_type ndx = 0; ndx < limit; ++ndx) {
      elems_[ndx] = data[ndx];
    }
  }

  iterator begin() { return elems_; }
  const_iterator begin() const { return elems_; }
  iterator end() { return elems_ + SIZE; }
  const_iterator end() const { return elems_ + SIZE; }

  // Returns the number of elements in the array.
  constexpr size_type size() const { return SIZE; }
  constexpr bool empty() const { return false; }

  // Element access:
  reference operator[](size_type ndx) {
    TAS_DCHECK_LT(ndx, SIZE);
    return elems_[ndx];
  }
  const_reference operator[](size_type ndx) const {
    TAS_DCHECK_LT(ndx, SIZE);
    return elems_[ndx];
  }
  reference at(size_type ndx) {
    TAS_DCHECK_LT(ndx, SIZE);
    return elems_[ndx];
  }
  const_reference at(size_type ndx) const {
    TAS_DCHECK_LT(ndx, SIZE);
    return elems_[ndx];
  }

  // Returns a pointer to the first element of the underlying array.
  reference front() { return elems_[0]; }
  const_reference front() const { return elems_[0]; }
  reference back() { return elems_[SIZE - 1]; }
  const_reference back() const { return elems_[SIZE - 1]; }

  // Returns a pointer to the first element of the underlying array.
  pointer data() { return elems_; }
  constexpr const_pointer data() const { return elems_; }

  // The actual data.
  value_type elems_[SIZE];  // NOLINT
};

// Helpers to build an Array from some values.
template <typename T, int SIZE>
Array<T, SIZE> MakeFromArray(const T (&data)[SIZE]) {
  Array<T, SIZE> array;
  array.copy(data);
  return array;
}

template <typename T>
constexpr Array<T, 1> MakeArray(T a) {
  return {a};
}

template <typename T>
constexpr Array<T, 2> MakeArray(T a, T b) {
  return {a, b};
}

template <typename T>
constexpr Array<T, 3> MakeArray(T a, T b, T c) {
  return {a, b, c};
}

template <typename T>
constexpr Array<T, 4> MakeArray(T a, T b, T c, T d) {
  return {a, b, c, d};
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_ARRAY_H_

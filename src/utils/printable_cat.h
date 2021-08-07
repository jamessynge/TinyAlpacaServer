#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PRINTABLE_CAT_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PRINTABLE_CAT_H_

// PrintableCat provides the ability to create a Printable instance that
// concatenates multiple Printable objects, esp. strings and numbers. The intent
// is to use this for creating error messages assembled from multiple parts.
// This is inspired by absl::StrCat, but does not perform allocation. Instead it
// addresses the fact that allocation is "hard" on an Arduino, except for stack
// variables. Therefore it creates an AnyPrintableArray<N>, holding an array of
// N AnyPrintable instances.
//
// Author: james.synge@gmail.com

#include "mcucore_platform.h"
#include "utils/any_printable.h"
#include "utils/array.h"

namespace alpaca {
namespace internal {
size_t PrintAnyPrintablesTo(const AnyPrintable* printables,
                            size_t num_printables, Print& out);
}  // namespace internal

template <size_t SIZE>
struct AnyPrintableArray : public Printable {
  explicit AnyPrintableArray(const Array<AnyPrintable, SIZE>& printables)
      : printables_(printables) {}
  explicit AnyPrintableArray(Array<AnyPrintable, SIZE>&& printables)
      : printables_(printables) {}

  AnyPrintableArray(const AnyPrintableArray<SIZE>&) = default;
  AnyPrintableArray(AnyPrintableArray<SIZE>&&) = default;

  size_t printTo(Print& out) const override {
    return internal::PrintAnyPrintablesTo(printables_.data(), SIZE, out);
  }

  Array<AnyPrintable, SIZE> printables_;  // NOLINT
};

// TODO(jamessynge): Better understand template argument deduction, and whether
// there are ways to have the compiler deduce the SIZE of the array.
template <class A>
AnyPrintableArray<1> PrintableCat(A a) {
  return AnyPrintableArray<1>(Array<AnyPrintable, 1>{AnyPrintable(a)});
}

template <class A, class B>
AnyPrintableArray<2> PrintableCat(A a, B b) {
  return AnyPrintableArray<2>(
      Array<AnyPrintable, 2>{AnyPrintable(a), AnyPrintable(b)});
}

template <class A, class B, class C>
AnyPrintableArray<3> PrintableCat(A a, B b, C c) {
  return AnyPrintableArray<3>(Array<AnyPrintable, 3>{
      AnyPrintable(a), AnyPrintable(b), AnyPrintable(c)});
}

template <class A, class B, class C, class D>
AnyPrintableArray<4> PrintableCat(A a, B b, C c, D d) {
  return AnyPrintableArray<4>(Array<AnyPrintable, 4>{
      AnyPrintable(a), AnyPrintable(b), AnyPrintable(c), AnyPrintable(d)});
}

template <class A, class B, class C, class D, class E>
AnyPrintableArray<5> PrintableCat(A a, B b, C c, D d, E e) {
  return AnyPrintableArray<5>(
      Array<AnyPrintable, 5>{AnyPrintable(a), AnyPrintable(b), AnyPrintable(c),
                             AnyPrintable(d), AnyPrintable(e)});
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PRINTABLE_CAT_H_

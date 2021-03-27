#ifndef TINY_ALPACA_SERVER_SRC_UTILS_TYPE_TRAITS_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_TYPE_TRAITS_H_

// The minimal parts of <type_traits>, etc., needed for Tiny Alpaca Server. This
// is compiled with avr-gcc, which is mostly C++ 11 compatible, but without the
// Standard Template Library.

namespace alpaca {
namespace tt_internal {
// Based on https://en.cppreference.com/w/cpp/types/add_reference:

template <class T>
struct type_identity {
  using type = T;
};

template <class T>
auto try_add_lvalue_reference(int) -> type_identity<T&>;
template <class T>
auto try_add_lvalue_reference(...) -> type_identity<T>;

template <class T>
auto try_add_rvalue_reference(int) -> type_identity<T&&>;
template <class T>
auto try_add_rvalue_reference(...) -> type_identity<T>;

}  // namespace tt_internal

template <class T>
struct add_lvalue_reference
    : decltype(tt_internal::try_add_lvalue_reference<T>(0)) {};

template <class T>
struct add_rvalue_reference
    : decltype(tt_internal::try_add_rvalue_reference<T>(0)) {};

// Based on https://en.cppreference.com/w/cpp/utility/declval

template <class T>
typename add_rvalue_reference<T>::type declval() noexcept;

// Based on https://en.cppreference.com/w/cpp/types/integral_constant

template <class T, T t_value>
struct integral_constant {
  static constexpr T value = t_value;
  using value_type = T;
  using type = integral_constant;  // using injected-class-name
  constexpr operator value_type() const noexcept { return value; }  // NOLINT
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

// Based on http://coliru.stacked-crooked.com/a/cd139d95d214c5c3,
// and https://stackoverflow.com/a/9154394.

template <class>
struct sfinae_true : true_type {};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_TYPE_TRAITS_H_

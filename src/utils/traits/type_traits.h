#ifndef TINY_ALPACA_SERVER_SRC_UTILS_TRAITS_TYPE_TRAITS_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_TRAITS_TYPE_TRAITS_H_

// The minimal parts of STL's <type_traits>, etc., needed for Tiny Alpaca
// Server. This is compiled with avr-gcc, which is mostly C++ 11 compatible, but
// without the Standard Template Library.

#include "utils/platform.h"

namespace alpaca {

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

// Based on https://en.cppreference.com/w/cpp/types/remove_cv

template <class T>
struct remove_cv {
  typedef T type;
};
template <class T>
struct remove_cv<const T> {
  typedef T type;
};
template <class T>
struct remove_cv<volatile T> {
  typedef T type;
};
template <class T>
struct remove_cv<const volatile T> {
  typedef T type;
};

template <class T>
struct remove_const {
  typedef T type;
};
template <class T>
struct remove_const<const T> {
  typedef T type;
};

template <class T>
struct remove_volatile {
  typedef T type;
};
template <class T>
struct remove_volatile<volatile T> {
  typedef T type;
};

template <class T>
using remove_cv_t = typename remove_cv<T>::type;
template <class T>
using remove_const_t = typename remove_const<T>::type;
template <class T>
using remove_volatile_t = typename remove_volatile<T>::type;

// Based on https://stackoverflow.com/a/43571992
// Behaves mostly like std::is_integral ...

namespace tt_internal {

template <typename>
struct test_is_integral : false_type {};

#define TYPE_TRAITS_IS_INTEGRAL_HELPER_(base_name) \
  template <>                                      \
  struct test_is_integral<base_name> : true_type {}

TYPE_TRAITS_IS_INTEGRAL_HELPER_(bool);
TYPE_TRAITS_IS_INTEGRAL_HELPER_(int8_t);
TYPE_TRAITS_IS_INTEGRAL_HELPER_(uint8_t);
TYPE_TRAITS_IS_INTEGRAL_HELPER_(int16_t);
TYPE_TRAITS_IS_INTEGRAL_HELPER_(uint16_t);
TYPE_TRAITS_IS_INTEGRAL_HELPER_(int32_t);
TYPE_TRAITS_IS_INTEGRAL_HELPER_(uint32_t);

#undef TYPE_TRAITS_IS_INTEGRAL_HELPER_
}  // namespace tt_internal

// is_integral extends either true_type or false_type, depending on whether T
// is an integral type.
template <typename T>
struct is_integral : decltype(tt_internal::test_is_integral<remove_cv_t<T>>()) {
};

// C++ 17 feature
template <typename T>
inline constexpr bool is_integral_v = is_integral<T>::value;

// Based on https://en.cppreference.com/w/cpp/types/add_reference:

namespace tt_internal {

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

// Based on https://en.cppreference.com/w/cpp/types/is_pointer

namespace tt_internal {

template <class T>
struct is_pointer_helper : false_type {};

template <class T>
struct is_pointer_helper<T*> : true_type {};

}  // namespace tt_internal

template <class T>
struct is_pointer
    : tt_internal::is_pointer_helper<typename remove_cv<T>::type> {};

// Based on http://coliru.stacked-crooked.com/a/cd139d95d214c5c3,
// and https://stackoverflow.com/a/9154394.

template <class>
struct sfinae_true : true_type {};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_TRAITS_TYPE_TRAITS_H_

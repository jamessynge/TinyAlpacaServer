#ifndef TINY_ALPACA_SERVER_SRC_UTILS_TRAITS_PRINT_TO_TRAIT_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_TRAITS_PRINT_TO_TRAIT_H_

// Support for determining at compile time if a value has support for being
// inserted into an OPrintStream. In particular, is it a class instance with a
// printTo method that has a signature compatible with Arduino's
// Printable::printTo method, or is there a PrintValueTo function that will
// print the value to a Print instance.
//
// Author: james.synge@gmail.com

#include "utils/platform.h"
#include "utils/traits/type_traits.h"

namespace alpaca {
namespace print_to_trait_internal {

// Matches a T that has a printTo(Print&) member function.
template <class T>
static auto test_print_to(int)
    -> sfinae_true<decltype(declval<T>().printTo(declval<::Print&>()))>;

// SFINAE fallback for the case where T does not have a printTo(Print&) member
// function. This depends on the fact that type of the literal '0' is int, so
// the compiler will prefer a match to the prior function, but will fallback to
// this one instead of emitting an error.
template <typename T>
static auto test_print_to(long) -> false_type;  // NOLINT

}  // namespace print_to_trait_internal

// has_print_to extends either true_type or false_type, depending on whether T
// has a printTo(Print&) member function.
template <typename T>
struct has_print_to : decltype(print_to_trait_internal::test_print_to<T>(0)) {};

// has_print_value_to extends either true_type or false_type, depending on
// whether there exists a PrintValueTo(T, Print&) function. The first definition
// matches any type for which there is not a corresponding PrintValueTo
// function.
template <class, class = void>
struct has_print_value_to : false_type {};

// Matches a T for which there is a PrintValueTo(T, Print&) function.
template <class T>
struct has_print_value_to<
    T, void_t<decltype(PrintValueTo(declval<T>(), declval<::Print&>()))>>
    : true_type {};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_TRAITS_PRINT_TO_TRAIT_H_

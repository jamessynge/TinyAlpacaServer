#ifndef SRC_UTILS_PRINT_TO_TRAIT_H
#define SRC_UTILS_PRINT_TO_TRAIT_H

// Support for streaming into a Print instance, primarily for logging.

#include "utils/platform.h"
#include "utils/type_traits.h"

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

}  // namespace alpaca

#endif // SRC_UTILS_PRINT_TO_TRAIT_H

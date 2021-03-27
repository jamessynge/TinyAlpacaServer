#ifndef TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_

// Support for streaming into a Print instance, primarily for logging.

#include "utils/platform.h"
#include "utils/type_traits.h"

namespace alpaca {
namespace ops_internal {

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

}  // namespace ops_internal

// has_print_to extends either true_type or false_type, depending on whether T
// has a printTo(Print&) member function.
template <typename T>
struct has_print_to : decltype(ops_internal::test_print_to<T>(0)) {};

class OPrintStream {
 public:
  explicit OPrintStream(Print& out) : out_(out) {}

  template <typename T>
  friend OPrintStream& operator<<(OPrintStream& out, const T& value) {
    // Selects the specialization of OPrintStream::print based on whether T
    // has a printTo(Print&) member function.
    OPrintStream::print(out.out_, value, has_print_to<T>{});
    return out;
  }

 protected:
  template <typename T>
  static void print(Print& out, const T& value, true_type /*has_print_to*/) {
    value.printTo(out);
  }
  template <typename T>
  static void print(Print& out, const T& value, false_type /*!has_print_to*/) {
    out.print(value);
  }

  Print& out_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_

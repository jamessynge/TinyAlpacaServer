#ifndef TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_

// Support for streaming into a Print instance, primarily for logging.

#include "utils/platform.h"
#include "utils/print_to_trait.h"

namespace alpaca {

class OPrintStream {
 public:
  explicit OPrintStream(Print& out) : out_(out) {}

  template <typename T>
  friend OPrintStream& operator<<(OPrintStream& out, const T& value) {
    // Selects the specialization of OPrintStream::print based on whether T
    // has a printTo(Print&) member function.
    OPrintStream::do_print(out.out_, value, has_print_to<T>{});
    return out;
  }

  template <typename T>
  friend OPrintStream& operator<<(OPrintStream&& out, const T& value) {
    // Selects the specialization of OPrintStream::print based on whether T
    // has a printTo(Print&) member function.
    OPrintStream::do_print(out.out_, value, has_print_to<T>{});
    return out;
  }

 protected:
  template <typename T>
  static void do_print(Print& out, const T& value, true_type /*has_print_to*/) {
    value.printTo(out);
  }
  template <typename T>
  static void do_print(Print& out, const T& value,
                       false_type /*!has_print_to*/) {
    out.print(value);
  }

  Print& out_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_

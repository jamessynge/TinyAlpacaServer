#ifndef TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_

// Support for streaming into a Print instance, primarily for logging.

#include "utils/platform.h"
#include "utils/traits/print_to_trait.h"
#include "utils/traits/type_traits.h"

namespace alpaca {

class OPrintStream;

using OPrintStreamManipulator = void (*)(OPrintStream&);

class OPrintStream {
 public:
  explicit OPrintStream(Print& out) : out_(out), base_(10) {}

  template <typename T>
  friend OPrintStream& operator<<(OPrintStream& out, const T value) {
    out.do_print_a(value, has_print_to<T>{});
    return out;
  }

  template <typename T>
  friend OPrintStream& operator<<(OPrintStream&& out, const T value) {
    out.do_print_a(value, has_print_to<T>{});
    return out;
  }

  void set_base(int base) { base_ = base; }

 protected:
  Print& out_;
  int base_;

 private:
  // T is a class with a printTo function.
  template <typename T>
  void do_print_a(const T value, true_type /*has_print_to*/) {
    value.printTo(out_);
  }

  // Type T does NOT have a printTo method.
  template <typename T>
  void do_print_a(const T value, false_type /*!has_print_to*/) {
    do_print_b(value, is_integral<T>{});
  }

  // Type T is an integral type (includes bool and char).
  template <typename T>
  void do_print_b(const T value, true_type /*is_integral*/) {
    out_.print(value, base_);
  }

  // Type T is NOT an integral type.
  template <typename T>
  void do_print_b(const T value, false_type /*!is_integral*/) {
    do_print_c(value, is_pointer<T>{});
  }

  void do_print_c(const char* value, true_type /*is_pointer*/) {
    out_.print(value);
  }

  void do_print_c(OPrintStreamManipulator manipulator,
                  true_type /*is_pointer*/) {
    (*manipulator)(*this);
  }

  template <typename T>
  void do_print_c(const T value, false_type /*is_pointer*/) {
    out_.print(value);
  }
};

inline void BaseDec(OPrintStream& strm) { strm.set_base(10); }
inline void BaseHex(OPrintStream& strm) { strm.set_base(16); }

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_

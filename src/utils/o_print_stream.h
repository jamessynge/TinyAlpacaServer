#ifndef TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_

// Support for streaming into a Print instance, primarily for logging.
//
// Author: james.synge@gmail.com

#include "utils/platform.h"
#include "utils/traits/print_to_trait.h"
#include "utils/traits/type_traits.h"

#if TAS_HOST_TARGET
#include <string>
#endif

namespace alpaca {

class OPrintStream {
 public:
  // An OPrintStreamManipulator is a function which can modify the OPrintStream
  // instance. So far the only two are BaseDec and BaseHex, allowing a logging
  // statement to choose the base for printing numbers.
  using OPrintStreamManipulator = void (*)(OPrintStream&);

  explicit OPrintStream(Print& out) : out_(out), base_(10) {}
  OPrintStream() : out_(::Serial), base_(10) {}

  template <typename T>
  OPrintStream& operator<<(const T value) {
    do_print_a(value, has_print_to<T>{});
    return *this;
  }

  // Set the base in which numbers are printed.
  void set_base(int base) { base_ = base; }

 protected:
  // Exposed so that subclasses can call this.
  template <typename T>
  void printValue(const T value) {
    do_print_a(value, has_print_to<T>{});
  }

  Print& out_;
  int base_;

 private:
  // I'm using type traits to steer the call to the appropriate method or
  // function for printing a value of type T.

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
    do_print_c(value, has_print_value_to<T>{});
  }

  template <typename T>
  void do_print_c(const T value, true_type /*has_print_value_to*/) {
    PrintValueTo(value, out_);
  }

  template <typename T>
  void do_print_c(const T value, false_type /*has_print_value_to*/) {
    do_print_d(value, is_pointer<T>{});
  }

  void do_print_d(const char* value, true_type /*is_pointer*/) {
    out_.print(value);
  }

  void do_print_d(OPrintStreamManipulator manipulator,
                  true_type /*is_pointer*/) {
    (*manipulator)(*this);
  }

  void do_print_d(const void* misc_pointer, true_type /*is_pointer*/) {
    auto i = reinterpret_cast<const uintptr_t>(misc_pointer);
    out_.print(i, HEX);
  }

#if TAS_HOST_TARGET
  void do_print_d(const std::string& value, false_type /*!is_pointer*/) {
    out_.write(value.data(), value.size());
  }
#endif

  template <typename T>
  void do_print_d(const T value, false_type /*!is_pointer*/) {
    out_.print(value);
  }
};

// Set the base for printing numbers to 16. For example:
//
//   strm << "In hex: " << BaseHex << 123;
//
// Will insert "In hex: 7B" into strm.
inline void BaseHex(OPrintStream& strm) { strm.set_base(16); }

// Set the base for printing numbers to 10 (the default). For example:
//
//   strm << BaseHex << "Value: " << 123 << BaseDec << ", " << 123;
//
// Will insert "Value: 7B, 123" into strm.
inline void BaseDec(OPrintStream& strm) { strm.set_base(10); }

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_

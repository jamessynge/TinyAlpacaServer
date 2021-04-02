#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LOG_SINK_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LOG_SINK_H_

// LogSink is used for printing a message (*line* of text) to a Print instance.
// VoidSink is used in place of LogSink when logging is disabled at compile
// time.

#ifndef ARDUINO
#include "logging.h"
#endif  // !ARDUINO

#include "utils/o_print_stream.h"
#include "utils/platform.h"
// #include
// "experimental/users/jamessynge/tiny-alpaca-server/src/utils/utils_config.h"

#ifndef ARDUINO
#ifndef NDEBUG
// Log the lifetime of VoidSink instances, as part of proving that only one is
// created per disabled TAS_VLOG.
#define NOISY_VOID_SINK
#endif
#endif
#define NOISY_VOID_SINK

namespace alpaca {

class LogSink : public OPrintStream {
 public:
  explicit LogSink(Print& out);
  LogSink();
  ~LogSink();
};

class CheckSink : public OPrintStream {
 public:
  CheckSink(Print& out, const char* expression_message);
  explicit CheckSink(const char* expression_message);
  ~CheckSink();

 private:
  const char* const expression_message_;
};

class VoidSink {
 public:
  VoidSink() {
#ifdef NOISY_VOID_SINK
    VLOG(3) << "VoidSink() ctor @ " << std::hex << this << std::dec
            << ", count=" << count_;
#endif  // !ARDUINO
  }
  VoidSink(const VoidSink&) {
#ifdef NOISY_VOID_SINK
    VLOG(3) << "VoidSink(const VoidSink&) ctor @ " << std::hex << this
            << std::dec << ", count=" << count_;
#endif  // !ARDUINO
  }
  VoidSink(VoidSink&&) {
#ifdef NOISY_VOID_SINK
    VLOG(3) << "VoidSink(VoidSink&&) ctor @ " << std::hex << this << std::dec
            << ", count=" << count_;
#endif  // !ARDUINO
  }
  ~VoidSink() {
#ifdef NOISY_VOID_SINK
    VLOG(3) << "VoidSink() dtor, " << count_;
#endif  // !ARDUINO
  }

  template <typename T>
  friend VoidSink& operator<<(const VoidSink& sink, const T&) {
#ifdef NOISY_VOID_SINK
    VLOG(3) << "operator<< const&";
#endif  // !ARDUINO
    return const_cast<VoidSink&>(sink);
  }

  template <typename T>
  friend VoidSink& operator<<(VoidSink& sink, const T&) {
#ifdef NOISY_VOID_SINK
    VLOG(3) << "operator<< &";
#endif  // !ARDUINO
    return sink;
  }

 private:
#ifdef NOISY_VOID_SINK
  int count_ = counter_++;
  static int counter_;
#endif  // !ARDUINO
};

// extern VoidSink TheVoidSink;

// Based on https://github.com/google/asylo/blob/master/asylo/util/logging.h
// This class is used just to take a type used as a log sink (i.e. the LHS of
// insert operators in log statements) make it a void type to satisify the
// ternary operator in TAS_VLOG, TAS_CHECK and TAS_DCHECK. `operand&&` is used
// because it has precedence lower than `<<` but higher than the ternary
// operator `:?`

class LogSinkVoidify {
 public:
  void operator&&(const OPrintStream&) {}
  void operator&&(const VoidSink&) {}
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LOG_SINK_H_

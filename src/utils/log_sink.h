#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LOG_SINK_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LOG_SINK_H_

// LogSink is used for printing a message (*line* of text) to a Print instance.
// VoidSink is used in place of LogSink when logging is disabled at compile
// time.

#include "utils/inline_literal.h"
#include "utils/o_print_stream.h"
#include "utils/platform.h"

namespace alpaca {

class LogSink : public OPrintStream {
 public:
  explicit LogSink(Print& out) : OPrintStream(out) {}
  LogSink() : LogSink(::Serial) {}
  ~LogSink();
};

class CheckSink : public OPrintStream {
 public:
  CheckSink(Print& out, PrintableProgmemString expression_message);
  explicit CheckSink(PrintableProgmemString expression_message);
  ~CheckSink();

 private:
  PrintableProgmemString expression_message_;
};

class VoidSink {
 public:
  VoidSink() {}
  VoidSink(const VoidSink&) {}
  VoidSink(VoidSink&&) {}
  ~VoidSink() {}

  template <typename T>
  VoidSink& operator<<(const T&) {
    return *this;
  }
};

// Based on https://github.com/google/asylo/blob/master/asylo/util/logging.h
// This class is used just to take a type used as a log sink (i.e. the LHS of
// insert operators in log statements) and make it a void type to satisify the
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

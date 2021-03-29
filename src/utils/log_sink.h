#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LOG_SINK_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LOG_SINK_H_

// LogSink is used for printing a message (*line* of text) to a Print instance.
// VoidSink is used in place of LogSink when logging is disabled at compile
// time.

#include "utils/platform.h"

#ifndef TAS_HOST_TARGET
#include "logging.h"
#endif 

#include "utils/o_print_stream.h"

namespace alpaca {

class LogSink : public OPrintStream {
 public:
  explicit LogSink(Print& out);
  LogSink();
  ~LogSink();
};

class CheckSink : public OPrintStream {
 public:
  explicit CheckSink(Print& out);
  CheckSink();
  ~CheckSink();
};

class VoidSink {
 public:
  VoidSink() {
#if TAS_HOST_TARGET
    DVLOG(3) << "VoidSink() ctor @ " << std::hex << this << std::dec
             << ", count=" << count_;
#endif
  }
  VoidSink(const VoidSink&) {
#if TAS_HOST_TARGET
    DVLOG(3) << "VoidSink(const VoidSink&) ctor @ " << std::hex << this
             << std::dec << ", count=" << count_;
#endif
  }
  VoidSink(VoidSink&&) {
#if TAS_HOST_TARGET
    DVLOG(3) << "VoidSink(VoidSink&&) ctor @ " << std::hex << this << std::dec
             << ", count=" << count_;
#endif
  }
  ~VoidSink() {
#if TAS_HOST_TARGET
    DVLOG(3) << "VoidSink() dtor, " << count_;
#endif
  }

  template <typename T>
  friend VoidSink& operator<<(const VoidSink& sink, const T&) {
#if TAS_HOST_TARGET
    DVLOG(3) << "operator<< const&";
#endif
    return const_cast<VoidSink&>(sink);
  }

  template <typename T>
  friend VoidSink& operator<<(VoidSink& sink, const T&) {
#if TAS_HOST_TARGET
    DVLOG(3) << "operator<< &";
#endif
    return sink;
  }

 private:
#if TAS_HOST_TARGET
  int count_ = counter_++;
  static int counter_;
#endif
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LOG_SINK_H_

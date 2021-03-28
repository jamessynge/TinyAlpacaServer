#include "utils/log_sink.h"

#include <cstdlib>

#include "base/logger.h"
#include "base/logging_extensions.h"
#include "utils/platform.h"

namespace alpaca {

LogSink::LogSink(Print& out) : OPrintStream(out) {
#if TAS_HOST_TARGET
  LOG(INFO) << "LogSink(Print&) ctor @" << std::hex << this;
#else
  Serial.print("LogSink(Print&) ctor @");
  Serial.println(static_cast<void*>(this), HEX)
#endif
}
LogSink::LogSink() : LogSink(::Serial) {
#if TAS_HOST_TARGET
  LOG(INFO) << "LogSink() ctor @" << std::hex << this;
#else
  Serial.print("LogSink() ctor @");
  Serial.println(static_cast<void*>(this), HEX)
#endif
}

LogSink::~LogSink() {
  out_.println();
#if TAS_HOST_TARGET
  LOG(INFO) << "LogSink dtor @" << std::hex << this;
#else
  Serial.print("LogSink() dtor @");
  Serial.println(static_cast<void*>(this), HEX)
#endif
}

CheckSink::CheckSink(Print& out) : OPrintStream(out) {
#if TAS_HOST_TARGET
  LOG(INFO) << "CheckSink(Print&) ctor @" << std::hex << this;
#else
  Serial.print("CheckSink(Print&) ctor @");
  Serial.println(static_cast<void*>(this), HEX)
#endif
}

CheckSink::CheckSink() : CheckSink(::Serial) {
#if TAS_HOST_TARGET
  LOG(INFO) << "CheckSink() ctor @" << std::hex << this;
#else
  Serial.print("CheckSink(Print&) ctor @");
  Serial.println(static_cast<void*>(this), HEX)
#endif
}

CheckSink::~CheckSink() {
  out_.println();
  out_.flush();
#ifdef ARDUINO
  while (true) {
    delay(10000);
    out_.println("CHECK FAILED");
    out_.flush();
  }
#else
  LOG(INFO) << "CheckSink dtor @" << std::hex << this;
  FlushLogFiles(base_logging::INFO);
  std::exit(1);
#endif
}

#if TAS_HOST_TARGET
int VoidSink::counter_;
#endif

}  // namespace alpaca

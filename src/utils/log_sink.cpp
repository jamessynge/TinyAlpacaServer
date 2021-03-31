#include "utils/log_sink.h"

// Logs the creation and deletiong of LogSink instances if NOISY_LOG_SINK is
// defined, and does the same for CheckSink instances if NOISY_CHECK_SINK is
// defined. These help with proving that only one instance is created per
// enabled TAS_VLOG, TAS_CHECK or TAS_DCHECK.

#ifndef NDEBUG
// #define NOISY_LOG_SINK
// #define NOISY_CHECK_SINK
#endif

#ifndef ARDUINO
#include "base/logging_extensions.h"
#include "logging.h"
#endif  // !ARDUINO

#include "utils/platform.h"

namespace alpaca {

LogSink::LogSink(Print& out) : OPrintStream(out) {
#ifdef NOISY_LOG_SINK
#ifdef ARDUINO
  Serial.print("LogSink(Print&) ctor @");
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "LogSink(Print&) ctor @" << std::hex << this;
#endif  // ARDUINO
#endif  // NOISY_LOG_SINK
}

LogSink::LogSink() : LogSink(::Serial) {
#ifdef NOISY_LOG_SINK
#ifdef ARDUINO
  Serial.print("LogSink() ctor @");
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "LogSink() ctor @" << std::hex << this;
#endif  // ARDUINO
#endif  // NOISY_LOG_SINK
}

LogSink::~LogSink() {
  out_.println();
  out_.flush();

#ifdef NOISY_LOG_SINK
#ifdef ARDUINO
  Serial.print("LogSink() dtor @");
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "LogSink dtor @" << std::hex << this;
#endif  // ARDUINO
#endif  // NOISY_LOG_SINK
}

CheckSink::CheckSink(Print& out, const char* expression_message)
    : OPrintStream(out), expression_message_(expression_message) {
  out.print("TAS_CHECK FAILED: ");
  out.println(expression_message_);
#ifdef NOISY_CHECK_SINK
#ifdef ARDUINO
  Serial.print("CheckSink(Print&, const char*) ctor @");
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "CheckSink(Print& out, const char* expression_message) ctor @"
            << std::hex << this;
#endif  // ARDUINO
#endif  // NOISY_CHECK_SINK
}

CheckSink::CheckSink(const char* expression_message)
    : CheckSink(::ToStdErr, expression_message) {
#ifdef NOISY_CHECK_SINK
#ifdef ARDUINO
  Serial.print("CheckSink(const char*) ctor @");
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "CheckSink(const char*) ctor @" << std::hex << this;
#endif  // ARDUINO
#endif  // NOISY_CHECK_SINK
}

CheckSink::~CheckSink() {
  out_.println();
  out_.flush();
#ifdef NOISY_CHECK_SINK
#ifdef ARDUINO
  Serial.print("CheckSink dtor @");
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "CheckSink dtor @" << std::hex << this;
#endif  // ARDUINO
#endif  // NOISY_CHECK_SINK

#ifdef ARDUINO
  while (true) {
    out.print("TAS_CHECK FAILED: ");
    out.println(expression_message_);
    out_.flush();
    delay(10000);
  }
#else   // !ARDUINO
  FlushLogFiles(base_logging::INFO);
  QCHECK(false) << "TAS_CHECK FAILED: " << expression_message_;
#endif  // ARDUINO
}

#ifdef NOISY_VOID_SINK
int VoidSink::counter_;
#endif  // NOISY_VOID_SINK

}  // namespace alpaca

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

#include "utils/inline_literal.h"

namespace alpaca {

LogSink::LogSink(Print& out) : OPrintStream(out) {
#ifdef NOISY_LOG_SINK
#ifdef ARDUINO
  Serial.print(TASLIT("LogSink(Print&) ctor @"));
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "LogSink(Print&) ctor @" << std::hex << this;
#endif  // ARDUINO
#endif  // NOISY_LOG_SINK
}

LogSink::LogSink() : LogSink(::Serial) {
#ifdef NOISY_LOG_SINK
#ifdef ARDUINO
  Serial.print(TASLIT("LogSink() ctor @"));
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
  Serial.print(TASLIT("LogSink() dtor @"));
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "LogSink dtor @" << std::hex << this;
#endif  // ARDUINO
#endif  // NOISY_LOG_SINK
}

CheckSink::CheckSink(Print& out, PrintableProgmemString expression_message)
    : OPrintStream(out), expression_message_(expression_message) {
  out.print(TASLIT("TAS_CHECK FAILED: "));
  out.println(expression_message_);
#ifdef NOISY_CHECK_SINK
#ifdef ARDUINO
  Serial.print(TASLIT("CheckSink(Print&, PrintableProgmemString) ctor @"));
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "CheckSink(Print&, PrintableProgmemString) ctor @" << std::hex
            << this;
#endif  // ARDUINO
#endif  // NOISY_CHECK_SINK
}

#ifdef ARDUINO
#define CheckSinkDestination ::Serial
#else
#define CheckSinkDestination ::ToStdErr
#endif

CheckSink::CheckSink(PrintableProgmemString expression_message)
    : CheckSink(CheckSinkDestination, expression_message) {
#ifdef NOISY_CHECK_SINK
#ifdef ARDUINO
  Serial.print(TASLIT("CheckSink(PrintableProgmemString) ctor @"));
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "CheckSink(PrintableProgmemString) ctor @" << std::hex << this;
#endif  // ARDUINO
#endif  // NOISY_CHECK_SINK
}

CheckSink::~CheckSink() {
  out_.println();
  out_.flush();
#ifdef NOISY_CHECK_SINK
#ifdef ARDUINO
  Serial.print(TASLIT("CheckSink dtor @"));
  Serial.println((unsigned int)this, HEX);
#else   // !ARDUINO
  LOG(INFO) << "CheckSink dtor @" << std::hex << this;
#endif  // ARDUINO
#endif  // NOISY_CHECK_SINK

#ifdef ARDUINO
  while (true) {
    out_.print(TASLIT("TAS_CHECK FAILED: "));
    out_.println(expression_message_);
    out_.flush();
    delay(10000);
  }
#else   // !ARDUINO
  FlushLogFiles(base_logging::INFO);
  CHECK(false) << "TAS_CHECK FAILED: " << expression_message_;
#endif  // ARDUINO
}

}  // namespace alpaca

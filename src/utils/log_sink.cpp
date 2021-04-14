#include "utils/log_sink.h"

#ifndef ARDUINO
#include "base/logging_extensions.h"
#include "logging.h"
#endif  // !ARDUINO

#include "utils/inline_literal.h"

namespace alpaca {

LogSink::~LogSink() {
  // End the line of output produced by the current TAS_VLOG statement.
  out_.println();
  out_.flush();
}

CheckSink::CheckSink(Print& out, PrintableProgmemString expression_message)
    : OPrintStream(out), expression_message_(expression_message) {
  out.print(TASLIT("TAS_CHECK FAILED: "));
  out.println(expression_message_);
}

#ifdef ARDUINO
#define CheckSinkDestination ::Serial
#else
#define CheckSinkDestination ::ToStdErr
#endif

CheckSink::CheckSink(PrintableProgmemString expression_message)
    : CheckSink(CheckSinkDestination, expression_message) {}

CheckSink::~CheckSink() {
  // End the line of output produced by the current TAS_*CHECK* statement.
  out_.println();
  out_.flush();

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

#include "utils/log_sink.h"

#include "utils/inline_literal.h"
#include "utils/platform.h"

#ifndef ARDUINO
#include "base/logging_extensions.h"
#include "extras/test_tools/print_to_std_string.h"  // pragma: keep extras include
#include "logging.h"
#endif  // !ARDUINO

namespace alpaca {
namespace {
// If file is provided, and has a '/' in it, return the location after the last
// occurrence of '/'.
const __FlashStringHelper* TrimPath(const __FlashStringHelper* file) {
  if (file != nullptr) {
    auto* last_slash = strrchr_P(PSTR(file), '/');
    if (last_slash != nullptr) {
      return FLASHSTR(last_slash + 1);
    }
  }
  return file;
}
}  // namespace

LogSink::~LogSink() {
  // End the line of output produced by the current TAS_VLOG statement.
  out_.println();
  out_.flush();
}

CheckSink::CheckSink(Print& out, const __FlashStringHelper* file,
                     uint16_t line_number,
                     const __FlashStringHelper* expression_message)
    : OPrintStream(out),
      file_(TrimPath(file)),
      expression_message_(expression_message),
      line_number_(line_number) {
  Announce(out);
}

#ifdef ARDUINO
#define CheckSinkDestination ::Serial
#else
#define CheckSinkDestination ::ToStdErr
#endif

CheckSink::CheckSink(const __FlashStringHelper* file, uint16_t line_number,
                     const __FlashStringHelper* expression_message)
    : CheckSink(CheckSinkDestination, file, line_number, expression_message) {}

CheckSink::~CheckSink() {
  // End the line of output produced by the current TAS_*CHECK* statement.
  out_.println();
  out_.flush();

#ifdef ARDUINO
  uint8_t seconds = 0;
  while (true) {
    delay(5000L + 5000L * seconds++);
    Announce(out_);
    out_.println();
    out_.flush();
  }
#else   // !ARDUINO
  FlushLogFiles(base_logging::INFO);
  PrintToStdString ptss;
  Announce(ptss);
  CHECK(false) << ptss.str();
#endif  // ARDUINO
}

void CheckSink::Announce(Print& out) const {
  out.print(TASLIT("TAS_CHECK FAILED: "));
  if (file_ != nullptr) {
    out.print(file_);
    if (line_number_ != 0) {
      out.print(':');
      out.print(line_number_);
    }
    out.print(']');
    out.print(' ');
  }
  out.print(expression_message_);
}

}  // namespace alpaca

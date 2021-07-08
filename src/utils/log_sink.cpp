#include "utils/log_sink.h"

#include "utils/inline_literal.h"
#include "utils/platform.h"

#ifndef ARDUINO
#include "base/logging_extensions.h"
#include "extras/test_tools/print_to_std_string.h"  // pragma: keep extras include
#include "logging.h"
#endif  // !ARDUINO

#ifdef ARDUINO
#define DEFAULT_SINK_OUT ::Serial
#else
#define DEFAULT_SINK_OUT ::ToStdErr
#endif

namespace alpaca {
namespace {
// If file is provided, and has a '/' in it, return the location after the last
// occurrence of '/'.
const __FlashStringHelper* TrimPath(const __FlashStringHelper* file) {
  if (file != nullptr) {
    auto* last_slash = strrchr_P(reinterpret_cast<const char*>(file), '/');
    if (last_slash != nullptr) {
      file = reinterpret_cast<decltype(file)>(last_slash + 1);
    }
  }
  return file;
}
}  // namespace

MessageSinkBase::MessageSinkBase(Print& out, const __FlashStringHelper* file,
                                 uint16_t line_number)
    : OPrintStream(out), file_(TrimPath(file)), line_number_(line_number) {}

void MessageSinkBase::PrintLocation(Print& out) const {
  if (file_ != nullptr) {
    out.print(file_);
    if (line_number_ != 0) {
      out.print(':');
      out.print(line_number_);
    }
    out.print(']');
    out.print(' ');
  }
}

LogSink::LogSink(Print& out, const __FlashStringHelper* file,
                 uint16_t line_number)
    : MessageSinkBase(out, file, line_number) {
  PrintLocation(out_);
}

LogSink::LogSink(const __FlashStringHelper* file, uint16_t line_number)
    : LogSink(DEFAULT_SINK_OUT, file, line_number) {}

LogSink::LogSink(Print& out) : LogSink(out, nullptr, 0) {}

LogSink::LogSink() : LogSink(DEFAULT_SINK_OUT) {}

LogSink::~LogSink() {
  // End the line of output produced by the active logging statement.
  out_.println();
  out_.flush();
}

CheckSink::CheckSink(Print& out, const __FlashStringHelper* file,
                     uint16_t line_number,
                     const __FlashStringHelper* expression_message)
    : MessageSinkBase(out, file, line_number),
      expression_message_(expression_message) {
  Announce(out);
}

CheckSink::CheckSink(const __FlashStringHelper* file, uint16_t line_number,
                     const __FlashStringHelper* expression_message)
    : CheckSink(DEFAULT_SINK_OUT, file, line_number, expression_message) {}

CheckSink::~CheckSink() {
  // End the line of output produced by the current TAS_*CHECK* statement.
  out_.println();
  out_.flush();

#ifdef ARDUINO
  uint8_t seconds = 0;
  while (true) {
    if (seconds < 255) {
      ++seconds;
    }
    delay(10000L + 10000L * seconds);
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
  out.print(TAS_FLASHSTR("TAS_CHECK FAILED: "));
  PrintLocation(out);
  out.print(expression_message_);
}

}  // namespace alpaca

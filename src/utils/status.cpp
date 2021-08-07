#include "utils/status.h"

#include "inline_literal.h"
#include "mcucore_platform.h"
#include "o_print_stream.h"
#include "utils/counting_print.h"
#include "utils/hex_escape.h"

namespace alpaca {

size_t Status::printTo(Print& out) const {
  CountingPrint counter(out);
  OPrintStream strm(counter);
  if (ok()) {
    strm << TAS_FLASHSTR("OK");
  } else {
    strm << TAS_FLASHSTR("{.code=}") << code_;
    if (message_.size()) {
      strm << TAS_FLASHSTR(", message=\"") << HexEscaped(message_) << '"';
    }
    strm << '}';
  }
  return counter.count();
}

}  // namespace alpaca

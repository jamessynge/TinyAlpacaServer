#ifndef TINY_ALPACA_SERVER_SRC_HTTP_RESPONSE_HEADER_H_
#define TINY_ALPACA_SERVER_SRC_HTTP_RESPONSE_HEADER_H_

// HttpResponseHeader holds the data needed to emit an HTTP response header.
// Should be entirely filled out before emitting (printing/streaming) the
// response, after which it should be const so that we can emit it multiple
// times if needed.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "constants.h"

namespace alpaca {

struct HttpResponseHeader : public Printable {
  HttpResponseHeader();

  void Reset();
  size_t printTo(Print& out) const override;

  EHttpStatusCode status_code;
  mcucore::ProgmemString reason_phrase;
  EContentType content_type;
  uint32_t content_length;
  bool do_close;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_HTTP_RESPONSE_HEADER_H_

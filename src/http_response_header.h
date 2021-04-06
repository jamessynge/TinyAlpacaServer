#ifndef TINY_ALPACA_SERVER_SRC_HTTP_RESPONSE_HEADER_H_
#define TINY_ALPACA_SERVER_SRC_HTTP_RESPONSE_HEADER_H_

// HttpResponseHeader holds the data needed to emit an HTTP response header.
// Should be entirely filled out before emitting (printing/streaming) the
// response, after which it should be const so that we can emit it multiple
// times if needed (unlikely).

#include "constants.h"
#include "utils/literal.h"
#include "utils/platform.h"

namespace alpaca {

struct HttpResponseHeader : public Printable {
  HttpResponseHeader();

  void Reset();
  size_t printTo(Print& out) const override;

  EHttpStatusCode status_code;
  Literal reason_phrase;
  EContentType content_type;
  uint16_t content_length;
  bool do_close;
};

// Declare stuff

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_HTTP_RESPONSE_HEADER_H_

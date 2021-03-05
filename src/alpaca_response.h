#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

// Functions for generating an HTTP response message to a Print instance.

#include "alpaca_request.h"
#include "any_string.h"
#include "json_encoder.h"
#include "platform.h"

namespace alpaca {

void WriteOkResponse(const JsonPropertySource& source, Print& out);

// The following write OK responses with JSON responses with a Value property
// of the indicated type.
void WriteArrayResponse(const AlpacaRequest& request,
                        const JsonElementSource& elements, Print& out);
void WriteArrayResponse(const AlpacaRequest& request,
                        const LiteralArray& literals, Print& out);
void WriteBoolResponse(const AlpacaRequest& request, bool value, Print& out);
void WriteDoubleResponse(const AlpacaRequest& request, double value,
                         Print& out);
void WriteFloatResponse(const AlpacaRequest& request, float value, Print& out);
void WriteStringResponse(const AlpacaRequest& request, const AnyString& value,
                         Print& out);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

// Functions for generating an HTTP response message to a Print instance.

#include "alpaca_request.h"
#include "utils/any_string.h"
#include "utils/json_encoder.h"
#include "utils/platform.h"

namespace alpaca {

// Writes to 'out' an OK response with a JSON body whose content is provided by
// 'source'.
void WriteOkResponse(const JsonPropertySource& source, Print& out);

// The following write to 'out' an OK response with JSON body whose 'Value'
// property is from the 'value' parameter, which is of the specified type.
void WriteArrayResponse(const AlpacaRequest& request,
                        const JsonElementSource& value, Print& out);
void WriteBoolResponse(const AlpacaRequest& request, bool value, Print& out);
void WriteDoubleResponse(const AlpacaRequest& request, double value,
                         Print& out);
void WriteFloatResponse(const AlpacaRequest& request, float value, Print& out);
void WriteIntegerResponse(const AlpacaRequest& request, uint32_t value,
                          Print& out);
void WriteIntegerResponse(const AlpacaRequest& request, int32_t value,
                          Print& out);
void WriteLiteralArrayResponse(const AlpacaRequest& request,
                               const LiteralArray& value, Print& out);
void WriteStringResponse(const AlpacaRequest& request, const AnyString& value,
                         Print& out);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

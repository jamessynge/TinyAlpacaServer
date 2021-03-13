#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

// Functions for generating an HTTP response message to a Print instance.

#include "alpaca_request.h"
#include "constants.h"
#include "utils/any_printable.h"
#include "utils/any_string.h"
#include "utils/json_encoder.h"
#include "utils/platform.h"
#include "utils/status_or.h"

namespace alpaca {

// Writes to 'out' an OK response with a JSON body whose content is provided by
// 'source'. If http_method==HEAD, then the body is not written, but the header
// contains the content-length that would be send for a GET request. Returns
// true if there is no problem with writing the response.
bool WriteOkResponse(const JsonPropertySource& source, EHttpMethod http_method,
                     Print& out);

// The following WriteXyzResponse write to 'out' an OK response with JSON body
// whose 'Value' property is from the 'value' parameter, which is of the
// specified type. Returns true if there is no problem with writing the
// response.

bool WriteArrayResponse(const AlpacaRequest& request,
                        const JsonElementSource& value, Print& out);

bool WriteBoolResponse(const AlpacaRequest& request, bool value, Print& out);
bool WriteBoolResponse(const AlpacaRequest& request,
                       StatusOr<bool> status_or_value, Print& out);

bool WriteDoubleResponse(const AlpacaRequest& request, double value,
                         Print& out);
bool WriteDoubleResponse(const AlpacaRequest& request,
                         StatusOr<double> status_or_value, Print& out);

bool WriteFloatResponse(const AlpacaRequest& request, float value, Print& out);
bool WriteFloatResponse(const AlpacaRequest& request,
                        StatusOr<float> status_or_value, Print& out);

bool WriteIntegerResponse(const AlpacaRequest& request, uint32_t value,
                          Print& out);
bool WriteIntegerResponse(const AlpacaRequest& request,
                          StatusOr<uint32_t> status_or_value, Print& out);

bool WriteIntegerResponse(const AlpacaRequest& request, int32_t value,
                          Print& out);
bool WriteIntegerResponse(const AlpacaRequest& request,
                          StatusOr<int32_t> status_or_value, Print& out);

bool WriteLiteralArrayResponse(const AlpacaRequest& request,
                               const LiteralArray& value, Print& out);
// bool WriteLiteralArrayResponse(const AlpacaRequest& request,
//                                StatusOr<LiteralArray> status_or_value,
//                                Print& out);

bool WriteStringResponse(const AlpacaRequest& request, AnyString value,
                         Print& out);
bool WriteStringResponse(const AlpacaRequest& request, Printable& value,
                         Print& out);
// bool WriteStringResponse(const AlpacaRequest& request,
//                          StatusOr<AnyString> status_or_value, Print& out);

// Writes an ASCOM error response JSON body in an HTTP OK response message.
// Returns true if there is no problem with writing the response.
bool WriteAscomErrorResponse(const AlpacaRequest& request,
                             uint32_t error_number, AnyString error_message,
                             Print& out);
bool WriteAscomErrorResponse(const AlpacaRequest& request,
                             uint32_t error_number, Printable& error_message,
                             Print& out);
// Write the ASCOM error contained in the Status instance.
bool WriteAscomErrorResponse(const AlpacaRequest& request, Status error_status,
                             Print& out);
bool WriteAscomNotImplementedErrorResponse(const AlpacaRequest& request,
                                           Print& out);

// Writes an HTTP error response with a text body to out. Returns false.
bool WriteHttpErrorResponse(EHttpStatusCode status_code, const Printable& body,
                            Print& out);
bool WriteHttpErrorResponse(EHttpStatusCode status_code,
                            const AnyPrintable& body, Print& out);

// Writes a 500 Internal Server Error response with a text body to out. Returns
// false.
bool WriteHttpInternalServerError(Printable& body, Print& out);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

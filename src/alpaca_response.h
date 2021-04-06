#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

// Functions for generating an HTTP response message to a Print instance.

#include "alpaca_request.h"
#include "constants.h"
#include "utils/any_printable.h"
#include "utils/json_encoder.h"
#include "utils/platform.h"
#include "utils/status_or.h"

namespace alpaca {

struct WriteResponse {
  // Writes to 'out' an OK response with a JSON body whose content is provided
  // by 'source'. If request.http_method==HEAD, then the body is not written,
  // but the header contains the content-length that would be send for a GET
  // request. If request.do_close is true, then a "Connection: close" header is
  // added. Returns true if there is no problem with writing the response AND
  // request.do_close == false.
  static bool OkResponse(const AlpacaRequest& request,
                         const JsonPropertySource& source, Print& out);

  // The following WriteXyzResponse write to 'out' an OK response with JSON body
  // whose 'Value' property is from the 'value' parameter, which is of the
  // specified type. Returns true if there is no problem with writing the
  // response.

  static bool ArrayResponse(const AlpacaRequest& request,
                            const JsonElementSource& value, Print& out);

  static bool BoolResponse(const AlpacaRequest& request, bool value,
                           Print& out);
  static bool StatusOrBoolResponse(const AlpacaRequest& request,
                                   StatusOr<bool> status_or_value, Print& out);

  static bool DoubleResponse(const AlpacaRequest& request, double value,
                             Print& out);
  static bool StatusOrDoubleResponse(const AlpacaRequest& request,
                                     StatusOr<double> status_or_value,
                                     Print& out);

  static bool FloatResponse(const AlpacaRequest& request, float value,
                            Print& out);
  static bool StatusOrFloatResponse(const AlpacaRequest& request,
                                    StatusOr<float> status_or_value,
                                    Print& out);

  static bool UIntResponse(const AlpacaRequest& request, uint32_t value,
                           Print& out);
  static bool StatusOrUIntResponse(const AlpacaRequest& request,
                                   StatusOr<uint32_t> status_or_value,
                                   Print& out);

  static bool IntResponse(const AlpacaRequest& request, int32_t value,
                          Print& out);
  static bool StatusOrIntResponse(const AlpacaRequest& request,
                                  StatusOr<int32_t> status_or_value,
                                  Print& out);

  static bool LiteralArrayResponse(const AlpacaRequest& request,
                                   const LiteralArray& value, Print& out);
  // static bool LiteralArrayResponse(
  //     const AlpacaRequest& request,
  //     StatusOr<LiteralArray> status_or_value, Print& out);

  static bool StringResponse(const AlpacaRequest& request,
                             const AnyPrintable& value, Print& out);
  static bool StatusOrStringResponse(const AlpacaRequest& request,
                                     StatusOr<Literal> status_or_value,
                                     Print& out);

  // Writes an ASCOM error response JSON body in an HTTP OK response message.
  // Returns true if there is no problem with writing the response.
  static bool AscomErrorResponse(const AlpacaRequest& request,
                                 uint32_t error_number,
                                 const AnyPrintable& error_message, Print& out);

  // Write the ASCOM error contained in the Status instance.
  static bool AscomErrorResponse(const AlpacaRequest& request,
                                 Status error_status, Print& out);

  // Write an ASCOM Not Implemented error response.
  static bool AscomNotImplementedErrorResponse(const AlpacaRequest& request,
                                               Print& out);

  // Writes an HTTP error response with a text body to out. Returns false.
  static bool HttpErrorResponse(EHttpStatusCode status_code,
                                const Printable& body, Print& out);
  static bool HttpErrorResponse(EHttpStatusCode status_code, Literal body,
                                Print& out) {
    return HttpErrorResponse(status_code, AnyPrintable(body), out);
  }

  // Writes a 500 Internal Server Error response with a text body to out.
  // Returns false.
  static bool HttpInternalServerErrorResponse(Printable& body, Print& out);
};
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

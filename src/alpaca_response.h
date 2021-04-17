#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

// Functions for generating an HTTP response message to a Print instance.
//
// Author: james.synge@gmail.com

#include "alpaca_request.h"
#include "constants.h"
#include "utils/any_printable.h"
#include "utils/array_view.h"
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

  // The following XyzResponse methods write to 'out' an OK response with JSON
  // body whose 'Value' property is from the 'value' parameter, which is of the
  // specified type. Returns true if there is no problem with writing the
  // response.
  //
  // And the StatusOrXyzResponse methods write an OK response whose body depends
  // on the status_or_value argument:
  //
  // 1) If the status is OK, they delegate writing to XyzResponse;
  //
  // 2) If the status is not OK, then they delegate writing to
  //    AscomErrorResponse.

  static bool ArrayResponse(const AlpacaRequest& request,
                            const JsonElementSource& value, Print& out);

  static bool ObjectResponse(const AlpacaRequest& request,
                             const JsonPropertySource& value, Print& out);

  static bool BoolResponse(const AlpacaRequest& request, bool value,
                           Print& out);
  static bool StatusOrBoolResponse(const AlpacaRequest& request,
                                   StatusOr<bool> status_or_value, Print& out);

  static bool DoubleResponse(const AlpacaRequest& request, double value,
                             Print& out);
  static bool StatusOrDoubleResponse(const AlpacaRequest& request,
                                     StatusOr<double> status_or_value,
                                     Print& out);

  // TODO(jamessynge): Decide whether to keep the versions with float values.
  // Arduino's Print class only handles doubles, not floats, and the Alpaca API
  // specifies double as the floating point type, so this may be pointless.
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

  template <typename E>
  static bool StatusOrIntEnumResponse(const AlpacaRequest& request,
                                      StatusOr<E> status_or_value, Print& out) {
    if (status_or_value.ok()) {
      return IntResponse(request, static_cast<int32_t>(status_or_value.value()),
                         out);
    } else {
      return AscomErrorResponse(request, status_or_value.status(), out);
    }
  }

  // These methods aren't all called [StatusOr]StringResponse to make sure that
  // we don't make the mistake of returning a StatusOr<StringView>, or similar,
  // where the value to be captured is a reference (or contains a reference to)
  // a temporary that will disappear during the return (i.e. an example of a
  // memory ownership problem that Rust aims to address).
  static bool PrintableStringResponse(const AlpacaRequest& request,
                                      const Printable& value, Print& out);
  static bool AnyPrintableStringResponse(const AlpacaRequest& request,
                                         const AnyPrintable& value,
                                         Print& out) {
    return PrintableStringResponse(request, value, out);
  }
  static bool StatusOrLiteralResponse(const AlpacaRequest& request,
                                      StatusOr<Literal> status_or_value,
                                      Print& out);

  // Array responses.
  static bool UIntArrayResponse(const AlpacaRequest& request,
                                ArrayView<uint32_t> values, Print& out);
  static bool LiteralArrayResponse(const AlpacaRequest& request,
                                   const LiteralArray& value, Print& out);

  // Writes an ASCOM error response JSON body in an HTTP OK response message;
  // the header tells the client that the connection will be closed. Returns
  // false.
  static bool AscomErrorResponse(AlpacaRequest request, uint32_t error_number,
                                 const AnyPrintable& error_message, Print& out);
  static bool AscomErrorResponse(const AlpacaRequest& request,
                                 Status error_status, Print& out);

  // Write an ASCOM Action Not Implemented error response.
  static bool AscomActionNotImplementedResponse(const AlpacaRequest& request,
                                                Print& out);

  // Writes an HTTP error response with a text body to out. Returns false.
  static bool HttpErrorResponse(EHttpStatusCode status_code,
                                const Printable& body, Print& out);
  static bool HttpErrorResponse(EHttpStatusCode status_code, Literal body,
                                Print& out) {
    return HttpErrorResponse(status_code, AnyPrintable(body), out);
  }
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

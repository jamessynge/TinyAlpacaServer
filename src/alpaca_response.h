#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

// Functions for generating an HTTP response message to a Print instance.
//
// Author: james.synge@gmail.com

#include "alpaca_request.h"
#include "any_printable.h"
#include "array_view.h"
#include "constants.h"
#include "json_encoder.h"
#include "mcucore_platform.h"
#include "status_or.h"

namespace alpaca {

struct WriteResponse {
  // Writes to 'out' an OK response with the specified Content-Type, and with a
  // body whose content is provided 'content_source'. If
  // request.http_method==HEAD, then the body is not written, but the header
  // contains the content-length that would be sent for a GET request. If
  // request.do_close is true, then a "Connection: close" header is added.
  // Returns true if there is no problem with writing the response AND
  // request.do_close == false.
  static bool OkResponse(const AlpacaRequest& request,
                         EContentType content_type,
                         const Printable& content_source, Print& out,
                         bool append_http_newline = false);

  // Writes to 'out' an OK response with a JSON body whose content is provided
  // by 'source'. If request.http_method==HEAD, then the body is not written,
  // but the header contains the content-length that would be send for a GET
  // request. If request.do_close is true, then a "Connection: close" header is
  // added. Returns true if there is no problem with writing the response AND
  // request.do_close == false.
  static bool OkJsonResponse(const AlpacaRequest& request,
                             const mcucore::JsonPropertySource& source,
                             Print& out);

  // Writes to 'out' an OK response with an JSON body whose content is just the
  // minimal MethodResponse (i.e. ClientTransactionId, etc, and has no value).
  // If request.http_method==HEAD, then the body is not written, but the header
  // contains the content-length that would be send for a GET request. If
  // request.do_close is true, then a "Connection: close" header is added.
  // Returns true if there is no problem with writing the response AND
  // request.do_close == false.
  static bool StatusResponse(const AlpacaRequest& request,
                             mcucore::Status status, Print& out);

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
                            const mcucore::JsonElementSource& value,
                            Print& out);

  static bool ObjectResponse(const AlpacaRequest& request,
                             const mcucore::JsonPropertySource& value,
                             Print& out);

  static bool BoolResponse(const AlpacaRequest& request, bool value,
                           Print& out);
  static bool StatusOrBoolResponse(const AlpacaRequest& request,
                                   mcucore::StatusOr<bool> status_or_value,
                                   Print& out);

  static bool DoubleResponse(const AlpacaRequest& request, double value,
                             Print& out);
  static bool StatusOrDoubleResponse(const AlpacaRequest& request,
                                     mcucore::StatusOr<double> status_or_value,
                                     Print& out);

  // TODO(jamessynge): Decide whether to keep the versions with float values.
  // Arduino's Print class only handles doubles, not floats, and the Alpaca API
  // specifies double as the floating point type, so this may be pointless.
  static bool FloatResponse(const AlpacaRequest& request, float value,
                            Print& out);
  static bool StatusOrFloatResponse(const AlpacaRequest& request,
                                    mcucore::StatusOr<float> status_or_value,
                                    Print& out);

  static bool UIntResponse(const AlpacaRequest& request, uint32_t value,
                           Print& out);
  static bool StatusOrUIntResponse(const AlpacaRequest& request,
                                   mcucore::StatusOr<uint32_t> status_or_value,
                                   Print& out);

  static bool IntResponse(const AlpacaRequest& request, int32_t value,
                          Print& out);
  static bool StatusOrIntResponse(const AlpacaRequest& request,
                                  mcucore::StatusOr<int32_t> status_or_value,
                                  Print& out);

  template <typename E>
  static bool StatusOrIntEnumResponse(const AlpacaRequest& request,
                                      mcucore::StatusOr<E> status_or_value,
                                      Print& out) {
    if (status_or_value.ok()) {
      return IntResponse(request, static_cast<int32_t>(status_or_value.value()),
                         out);
    } else {
      return AscomErrorResponse(request, status_or_value.status(), out);
    }
  }

  static bool PrintableStringResponse(const AlpacaRequest& request,
                                      const Printable& value, Print& out);
  static bool AnyPrintableStringResponse(const AlpacaRequest& request,
                                         const mcucore::AnyPrintable& value,
                                         Print& out) {
    return PrintableStringResponse(request, value, out);
  }

  // These methods aren't all called [mcucore::StatusOr]StringResponse to make
  // sure that we don't make the mistake of returning a StatusOr<StringView>, or
  // similar, where the value to be captured is a reference (or contains a
  // reference to) a temporary that will disappear during the return (i.e. an
  // example of a memory ownership problem that Rust aims to address). For
  // example, if a device type adapter had a method such as:
  //
  //     virtual StatusOr<StringView> GetFoo() = 0;
  //
  // Then the characters referenced by the StringView might be on the stack, and
  // thus might go out of scope before the StatusOrStringViewResponse method
  // could be called; at that point we'd be attempting to generate a response
  // with garbage input.
  static bool StatusOrProgmemStringViewResponse(
      const AlpacaRequest& request,
      mcucore::StatusOr<mcucore::ProgmemStringView> status_or_value,
      Print& out);
  static bool StatusOrProgmemStringResponse(
      const AlpacaRequest& request,
      mcucore::StatusOr<mcucore::ProgmemString> status_or_value, Print& out);

  // Array responses.
  static bool UIntArrayResponse(const AlpacaRequest& request,
                                mcucore::ArrayView<uint32_t> values,
                                Print& out);
  static bool ProgmemStringArrayResponse(
      const AlpacaRequest& request, const mcucore::ProgmemStringArray& value,
      Print& out);

  // Writes an ASCOM error response JSON body in an HTTP OK response message;
  // the header tells the client that the connection will be closed. Returns
  // false.
  static bool AscomErrorResponse(AlpacaRequest request, uint32_t error_number,
                                 const Printable& error_message, Print& out);
  static bool AscomErrorResponse(AlpacaRequest request, uint32_t error_number,
                                 const mcucore::AnyPrintable& error_message,
                                 Print& out) {
    return AscomErrorResponse(request, error_number,
                              static_cast<const Printable&>(error_message),
                              out);
  }
  static bool AscomErrorResponse(const AlpacaRequest& request,
                                 mcucore::Status error_status, Print& out);

  static bool AscomParameterMissingErrorResponse(
      const AlpacaRequest& request, mcucore::ProgmemStringView parameter_name,
      Print& out);

  static bool AscomParameterInvalidErrorResponse(
      const AlpacaRequest& request, mcucore::ProgmemStringView parameter_name,
      Print& out);

  // Write an Alpaca Not Implemented error response, indicating that the
  // specified Alpaca method is not implemented.
  static bool AscomMethodNotImplementedResponse(
      const AlpacaRequest& request, const mcucore::AnyPrintable& method_name,
      Print& out);
  static bool AscomMethodNotImplementedResponse(const AlpacaRequest& request,
                                                EDeviceMethod method,
                                                Print& out);

  // As above, but the method is determined from the request.
  static bool AscomMethodNotImplementedResponse(const AlpacaRequest& request,
                                                Print& out);

  // Write an ASCOM Action Not Implemented error response.
  static bool AscomActionNotImplementedResponse(const AlpacaRequest& request,
                                                Print& out);

  // Writes an HTTP error response with a text body to out. Returns false.
  static bool HttpErrorResponse(EHttpStatusCode status_code,
                                const Printable& body, Print& out);
  static bool HttpErrorResponse(EHttpStatusCode status_code,
                                const mcucore::AnyPrintable& body, Print& out) {
    return HttpErrorResponse(status_code, static_cast<const Printable&>(body),
                             out);
  }
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_RESPONSE_H_

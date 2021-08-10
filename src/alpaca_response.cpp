#include "alpaca_response.h"

#include "any_printable.h"
#include "array_view.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "counting_print.h"
#include "http_response_header.h"
#include "json_encoder.h"
#include "json_encoder_helpers.h"
#include "json_response.h"
#include "literals.h"
#include "printable_cat.h"

namespace alpaca {
namespace {

class LiteralArraySource : public mcucore::JsonElementSource {
 public:
  explicit LiteralArraySource(const mcucore::LiteralArray& literals)
      : literals_(literals) {}
  void AddTo(mcucore::JsonArrayEncoder& encoder) const override {
    for (const mcucore::Literal& literal : literals_) {
      encoder.AddStringElement(literal);
    }
  }

 private:
  const mcucore::LiteralArray& literals_;
};

}  // namespace

bool WriteResponse::OkResponse(const AlpacaRequest& request,
                               EContentType content_type,
                               const Printable& content_source, Print& out,
                               bool append_http_newline) {
  const auto eol = Literals::HttpEndOfLine();
  HttpResponseHeader hrh;
  hrh.status_code = EHttpStatusCode::kHttpOk;
  hrh.reason_phrase = Literals::OK();
  hrh.content_type = content_type;
  hrh.content_length = mcucore::SizeOfPrintable(content_source);
  if (append_http_newline) {
    hrh.content_length += 2;
  }
  hrh.do_close = request.do_close;
  hrh.printTo(out);
  if (request.http_method != EHttpMethod::HEAD) {
    content_source.printTo(out);
    if (append_http_newline) {
      eol.printTo(out);
    }
  }
  return !request.do_close;
}

bool WriteResponse::OkJsonResponse(const AlpacaRequest& request,
                                   const mcucore::JsonPropertySource& source,
                                   Print& out) {
  mcucore::PrintableJsonObject content_source(source);
  return OkResponse(request, EContentType::kApplicationJson, content_source,
                    out, /*append_http_newline=*/true);
}

bool WriteResponse::StatusResponse(const AlpacaRequest& request,
                                   mcucore::Status status, Print& out) {
  if (status.ok()) {
    JsonMethodResponse body(request);
    return OkJsonResponse(request, body, out);
  } else {
    return AscomErrorResponse(request, status, out);
  }
}

bool WriteResponse::ArrayResponse(const AlpacaRequest& request,
                                  const mcucore::JsonElementSource& value,
                                  Print& out) {
  JsonArrayResponse source(request, value);
  return OkJsonResponse(request, source, out);
}

bool WriteResponse::ObjectResponse(const AlpacaRequest& request,
                                   const mcucore::JsonPropertySource& value,
                                   Print& out) {
  JsonObjectResponse source(request, value);
  return OkJsonResponse(request, source, out);
}

bool WriteResponse::BoolResponse(const AlpacaRequest& request, bool value,
                                 Print& out) {
  JsonBoolResponse source(request, value);
  return OkJsonResponse(request, source, out);
}

bool WriteResponse::StatusOrBoolResponse(
    const AlpacaRequest& request, mcucore::StatusOr<bool> status_or_value,
    Print& out) {
  if (status_or_value.ok()) {
    return BoolResponse(request, status_or_value.value(), out);
  } else {
    return AscomErrorResponse(request, status_or_value.status(), out);
  }
}

bool WriteResponse::DoubleResponse(const AlpacaRequest& request, double value,
                                   Print& out) {
  JsonDoubleResponse source(request, value);
  return OkJsonResponse(request, source, out);
}

bool WriteResponse::StatusOrDoubleResponse(
    const AlpacaRequest& request, mcucore::StatusOr<double> status_or_value,
    Print& out) {
  if (status_or_value.ok()) {
    return DoubleResponse(request, status_or_value.value(), out);
  } else {
    return AscomErrorResponse(request, status_or_value.status(), out);
  }
}

bool WriteResponse::FloatResponse(const AlpacaRequest& request, float value,
                                  Print& out) {
  JsonFloatResponse source(request, value);
  return OkJsonResponse(request, source, out);
}

bool WriteResponse::StatusOrFloatResponse(
    const AlpacaRequest& request, mcucore::StatusOr<float> status_or_value,
    Print& out) {
  if (status_or_value.ok()) {
    return FloatResponse(request, status_or_value.value(), out);
  } else {
    return AscomErrorResponse(request, status_or_value.status(), out);
  }
}

bool WriteResponse::UIntResponse(const AlpacaRequest& request, uint32_t value,
                                 Print& out) {
  JsonUnsignedIntegerResponse source(request, value);
  return OkJsonResponse(request, source, out);
}

bool WriteResponse::StatusOrUIntResponse(
    const AlpacaRequest& request, mcucore::StatusOr<uint32_t> status_or_value,
    Print& out) {
  if (status_or_value.ok()) {
    return UIntResponse(request, status_or_value.value(), out);
  } else {
    return AscomErrorResponse(request, status_or_value.status(), out);
  }
}

bool WriteResponse::IntResponse(const AlpacaRequest& request, int32_t value,
                                Print& out) {
  JsonIntegerResponse source(request, value);
  return OkJsonResponse(request, source, out);
}

bool WriteResponse::StatusOrIntResponse(
    const AlpacaRequest& request, mcucore::StatusOr<int32_t> status_or_value,
    Print& out) {
  if (status_or_value.ok()) {
    return IntResponse(request, status_or_value.value(), out);
  } else {
    return AscomErrorResponse(request, status_or_value.status(), out);
  }
}

bool WriteResponse::PrintableStringResponse(const AlpacaRequest& request,
                                            const Printable& value,
                                            Print& out) {
  JsonStringResponse source(request, value);
  return OkJsonResponse(request, source, out);
}

bool WriteResponse::StatusOrLiteralResponse(
    const AlpacaRequest& request,
    mcucore::StatusOr<mcucore::Literal> status_or_value, Print& out) {
  if (status_or_value.ok()) {
    return AnyPrintableStringResponse(request, status_or_value.value(), out);
  } else {
    return AscomErrorResponse(request, status_or_value.status(), out);
  }
}

bool WriteResponse::LiteralArrayResponse(const AlpacaRequest& request,
                                         const mcucore::LiteralArray& value,
                                         Print& out) {
  return ArrayResponse(request, LiteralArraySource(value), out);
}

bool WriteResponse::UIntArrayResponse(const AlpacaRequest& request,
                                      mcucore::ArrayView<uint32_t> values,
                                      Print& out) {
  return ArrayResponse(
      request,
      MakeArrayViewSource(values, &mcucore::JsonArrayEncoder::AddUIntElement),
      out);
}

bool WriteResponse::AscomErrorResponse(AlpacaRequest request,
                                       uint32_t error_number,
                                       const Printable& error_message,
                                       Print& out) {
  request.do_close = true;
  JsonMethodResponse source(request, error_number, error_message);
  return OkJsonResponse(request, source, out);
}

bool WriteResponse::AscomErrorResponse(const AlpacaRequest& request,
                                       mcucore::Status error_status,
                                       Print& out) {
  if (error_status.code() == ErrorCodes::kNotImplemented) {
    return AscomMethodNotImplementedResponse(request, out);
  }
  return AscomErrorResponse(request, error_status.code(),
                            mcucore::AnyPrintable(error_status.message()), out);
}

bool WriteResponse::AscomMethodNotImplementedResponse(
    const AlpacaRequest& request, const mcucore::AnyPrintable& method_name,
    Print& out) {
  auto error_message = mcucore::PrintableCat(
      TAS_FLASHSTR("Alpaca method not implemented: "), method_name);
  return AscomErrorResponse(request, ErrorCodes::kNotImplemented, error_message,
                            out);
}

bool WriteResponse::AscomMethodNotImplementedResponse(
    const AlpacaRequest& request, EDeviceMethod method, Print& out) {
  mcucore::AnyPrintable name(ToFlashStringHelper(method));
  return AscomMethodNotImplementedResponse(request, name, out);
}

bool WriteResponse::AscomMethodNotImplementedResponse(
    const AlpacaRequest& request, Print& out) {
  TAS_DCHECK(request.api == EAlpacaApi::kDeviceApi ||
             request.api == EAlpacaApi::kDeviceSetup);
  if (request.api == EAlpacaApi::kDeviceSetup) {
    return AscomMethodNotImplementedResponse(request, Literals::setup(), out);
  } else {
    return AscomMethodNotImplementedResponse(request, request.device_method,
                                             out);
  }
}

bool WriteResponse::AscomActionNotImplementedResponse(
    const AlpacaRequest& request, Print& out) {
  return AscomErrorResponse(request, ErrorCodes::ActionNotImplemented(), out);
}
bool WriteResponse::AscomParameterMissingErrorResponse(
    const AlpacaRequest& request, mcucore::Literal parameter_name, Print& out) {
  auto error_message = mcucore::PrintableCat(
      TAS_FLASHSTR("Missing parameter: "), parameter_name);
  return AscomErrorResponse(request, ErrorCodes::kValueNotSet, error_message,
                            out);
}

bool WriteResponse::AscomParameterInvalidErrorResponse(
    const AlpacaRequest& request, mcucore::Literal parameter_name, Print& out) {
  auto error_message = mcucore::PrintableCat(
      TAS_FLASHSTR("Invalid parameter: "), parameter_name);
  return AscomErrorResponse(request, ErrorCodes::kInvalidValue, error_message,
                            out);
}

bool WriteResponse::HttpErrorResponse(EHttpStatusCode status_code,
                                      const Printable& body, Print& out) {
  TAS_DCHECK_GE(status_code, EHttpStatusCode::kHttpBadRequest)
      << TAS_FLASHSTR("mcucore::Status code should be for an error.");

  HttpResponseHeader hrh;
  if (status_code < EHttpStatusCode::kHttpBadRequest) {
    hrh.status_code = EHttpStatusCode::kHttpInternalServerError;
    hrh.reason_phrase =
        TASLIT("Internal Server Error: Invalid HTTP mcucore::Status Code");
  } else {
    hrh.status_code = status_code;
    switch (status_code) {
      case EHttpStatusCode::kHttpBadRequest:
        hrh.reason_phrase = Literals::HttpBadRequest();
        break;
      case EHttpStatusCode::kHttpMethodNotAllowed:
        hrh.reason_phrase = Literals::HttpMethodNotAllowed();
        break;
      case EHttpStatusCode::kHttpNotAcceptable:
        hrh.reason_phrase = Literals::HttpNotAcceptable();
        break;
      case EHttpStatusCode::kHttpLengthRequired:
        hrh.reason_phrase = Literals::HttpLengthRequired();
        break;
      case EHttpStatusCode::kHttpPayloadTooLarge:
        hrh.reason_phrase = Literals::HttpPayloadTooLarge();
        break;
      case EHttpStatusCode::kHttpUnsupportedMediaType:
        hrh.reason_phrase = Literals::HttpUnsupportedMediaType();
        break;
      case EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge:
        hrh.reason_phrase = Literals::HttpRequestHeaderFieldsTooLarge();
        break;
      case EHttpStatusCode::kHttpMethodNotImplemented:
        hrh.reason_phrase = Literals::HttpMethodNotImplemented();
        break;
      case EHttpStatusCode::kHttpVersionNotSupported:
        hrh.reason_phrase = Literals::HttpVersionNotSupported();
        break;
      case EHttpStatusCode::kHttpInternalServerError:
        hrh.reason_phrase = Literals::HttpInternalServerError();
        break;
      default:
        // We don't have a reason phrase programmed in here.
        TAS_DCHECK(false) << TAS_FLASHSTR("Please add a case for status code ")
                          << status_code;
        hrh.reason_phrase = mcucore::Literal();
    }
  }

  hrh.content_type = EContentType::kTextPlain;
  hrh.content_length = mcucore::SizeOfPrintable(body);
  hrh.do_close = true;
  hrh.printTo(out);
  body.printTo(out);
  return false;
}

}  // namespace alpaca

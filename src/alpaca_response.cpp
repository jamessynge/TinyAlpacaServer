#include "alpaca_response.h"

#include "ascom_error_codes.h"
#include "constants.h"
#include "http_response_header.h"
#include "json_response.h"
#include "literals.h"
#include "utils/any_printable.h"
#include "utils/array_view.h"
#include "utils/counting_bitbucket.h"
#include "utils/json_encoder.h"
#include "utils/json_encoder_helpers.h"

namespace alpaca {
namespace {

class LiteralArraySource : public JsonElementSource {
 public:
  explicit LiteralArraySource(const LiteralArray& literals)
      : literals_(literals) {}
  void AddTo(JsonArrayEncoder& encoder) const override {
    for (const Literal& literal : literals_) {
      encoder.AddStringElement(literal);
    }
  }

 private:
  const LiteralArray& literals_;
};

}  // namespace

bool WriteResponse::OkResponse(const AlpacaRequest& request,
                               const JsonPropertySource& source, Print& out) {
  const auto eol = Literals::HttpEndOfLine();
  HttpResponseHeader hrh;
  hrh.status_code = EHttpStatusCode::kHttpOk;
  hrh.reason_phrase = Literals::OK();
  hrh.content_type = EContentType::kApplicationJson;
  hrh.content_length = JsonObjectEncoder::EncodedSize(source) + eol.size();
  hrh.do_close = request.do_close;
  hrh.printTo(out);
  if (request.http_method != EHttpMethod::HEAD) {
    JsonObjectEncoder::Encode(source, out);
    eol.printTo(out);
  }
  return !request.do_close;
}

bool WriteResponse::ArrayResponse(const AlpacaRequest& request,
                                  const JsonElementSource& value, Print& out) {
  JsonArrayResponse source(request, value);
  return OkResponse(request, source, out);
}

bool WriteResponse::BoolResponse(const AlpacaRequest& request, bool value,
                                 Print& out) {
  JsonBoolResponse source(request, value);
  return OkResponse(request, source, out);
}

bool WriteResponse::StatusOrBoolResponse(const AlpacaRequest& request,
                                         StatusOr<bool> status_or_value,
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
  return OkResponse(request, source, out);
}

bool WriteResponse::StatusOrDoubleResponse(const AlpacaRequest& request,
                                           StatusOr<double> status_or_value,
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
  return OkResponse(request, source, out);
}

bool WriteResponse::StatusOrFloatResponse(const AlpacaRequest& request,
                                          StatusOr<float> status_or_value,
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
  return OkResponse(request, source, out);
}

bool WriteResponse::StatusOrUIntResponse(const AlpacaRequest& request,
                                         StatusOr<uint32_t> status_or_value,
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
  return OkResponse(request, source, out);
}

bool WriteResponse::StatusOrIntResponse(const AlpacaRequest& request,
                                        StatusOr<int32_t> status_or_value,
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
  return OkResponse(request, source, out);
}

bool WriteResponse::StatusOrLiteralResponse(const AlpacaRequest& request,
                                            StatusOr<Literal> status_or_value,
                                            Print& out) {
  if (status_or_value.ok()) {
    return AnyPrintableStringResponse(request, status_or_value.value(), out);
  } else {
    return AscomErrorResponse(request, status_or_value.status(), out);
  }
}

bool WriteResponse::LiteralArrayResponse(const AlpacaRequest& request,
                                         const LiteralArray& value,
                                         Print& out) {
  return ArrayResponse(request, LiteralArraySource(value), out);
}

bool WriteResponse::UIntArrayResponse(const AlpacaRequest& request,
                                      ArrayView<uint32_t> values, Print& out) {
  return ArrayResponse(
      request, MakeArrayViewSource(values, &JsonArrayEncoder::AddUIntElement),
      out);
}

bool WriteResponse::AscomErrorResponse(AlpacaRequest request,
                                       uint32_t error_number,
                                       const AnyPrintable& error_message,
                                       Print& out) {
  request.do_close = true;
  JsonMethodResponse source(request, error_number, error_message);
  return OkResponse(request, source, out);
}

bool WriteResponse::AscomErrorResponse(const AlpacaRequest& request,
                                       Status error_status, Print& out) {
  return AscomErrorResponse(request, error_status.code(),
                            AnyPrintable(error_status.message()), out);
}

bool WriteResponse::AscomActionNotImplementedResponse(
    const AlpacaRequest& request, Print& out) {
  return AscomErrorResponse(request, ErrorCodes::ActionNotImplemented(), out);
}

bool WriteResponse::HttpErrorResponse(EHttpStatusCode status_code,
                                      const Printable& body, Print& out) {
  TAS_DCHECK_GE(status_code, EHttpStatusCode::kHttpBadRequest)
      << TASLIT("Status code should be for an error.");

  HttpResponseHeader hrh;
  if (status_code < EHttpStatusCode::kHttpBadRequest) {
    hrh.status_code = EHttpStatusCode::kHttpInternalServerError;

    auto taslit = TASLIT("Internal Server Error: Invalid HTTP Status Code");
    hrh.reason_phrase = Literal(taslit.progmem_data(), taslit.size());
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
        TAS_DCHECK(false) << "Please add a case for status code "
                          << status_code;
        hrh.reason_phrase = Literal();
    }
  }

  hrh.content_type = EContentType::kTextPlain;
  hrh.content_length = CountingBitbucket::SizeOfPrintable(body);
  hrh.do_close = true;
  hrh.printTo(out);
  body.printTo(out);
  return false;
}
}  // namespace alpaca

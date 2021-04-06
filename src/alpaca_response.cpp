#include "alpaca_response.h"

#include "ascom_error_codes.h"
#include "http_response_header.h"
#include "json_response.h"
#include "literals.h"
#include "utils/any_printable.h"
#include "utils/counting_bitbucket.h"
#include "utils/json_encoder.h"

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

// static
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

// static
bool WriteResponse::ArrayResponse(const AlpacaRequest& request,
                                  const JsonElementSource& value, Print& out) {
  JsonArrayResponse source(request, value);
  return OkResponse(request, source, out);
}

// static
bool WriteResponse::BoolResponse(const AlpacaRequest& request, bool value,
                                 Print& out) {
  JsonBoolResponse source(request, value);
  return OkResponse(request, source, out);
}

// static
bool WriteResponse::DoubleResponse(const AlpacaRequest& request, double value,
                                   Print& out) {
  JsonDoubleResponse source(request, value);
  return OkResponse(request, source, out);
}
// static
bool WriteResponse::StatusOrDoubleResponse(const AlpacaRequest& request,
                                           StatusOr<double> status_or_value,
                                           Print& out) {
  if (status_or_value.ok()) {
    return DoubleResponse(request, status_or_value.value(), out);
  } else {
    return AscomErrorResponse(request, status_or_value.status(), out);
  }
}

// static
bool WriteResponse::FloatResponse(const AlpacaRequest& request, float value,
                                  Print& out) {
  JsonFloatResponse source(request, value);
  return OkResponse(request, source, out);
}

// static
bool WriteResponse::StatusOrFloatResponse(const AlpacaRequest& request,
                                          StatusOr<float> status_or_value,
                                          Print& out) {
  if (status_or_value.ok()) {
    return FloatResponse(request, status_or_value.value(), out);
  } else {
    return AscomErrorResponse(request, status_or_value.status(), out);
  }
}

// static
bool WriteResponse::UIntResponse(const AlpacaRequest& request, uint32_t value,
                                 Print& out) {
  JsonUnsignedIntegerResponse source(request, value);
  return OkResponse(request, source, out);
}

// static
bool WriteResponse::IntResponse(const AlpacaRequest& request, int32_t value,
                                Print& out) {
  JsonIntegerResponse source(request, value);
  return OkResponse(request, source, out);
}

// static
bool WriteResponse::LiteralArrayResponse(const AlpacaRequest& request,
                                         const LiteralArray& value,
                                         Print& out) {
  return ArrayResponse(request, LiteralArraySource(value), out);
}

// static
bool WriteResponse::StringResponse(const AlpacaRequest& request,
                                   const AnyPrintable& value, Print& out) {
  JsonStringResponse source(request, value);
  return OkResponse(request, source, out);
}

// static
bool WriteResponse::StatusOrStringResponse(const AlpacaRequest& request,
                                           StatusOr<Literal> status_or_value,
                                           Print& out) {
  if (status_or_value.ok()) {
    return StringResponse(request, status_or_value.value(), out);
  } else {
    return AscomErrorResponse(request, status_or_value.status(), out);
  }
}

// static
bool WriteResponse::AscomErrorResponse(const AlpacaRequest& request,
                                       uint32_t error_number,
                                       const AnyPrintable& error_message,
                                       Print& out) {
  JsonMethodResponse source(request, error_number, error_message);
  OkResponse(request, source, out);
  return false;
}

// static
bool WriteResponse::AscomErrorResponse(const AlpacaRequest& request,
                                       Status error_status, Print& out) {
  return AscomErrorResponse(request, error_status.code(),
                            AnyPrintable(error_status.message()), out);
}

// static
bool WriteResponse::AscomNotImplementedErrorResponse(
    const AlpacaRequest& request, Print& out) {
  AscomErrorResponse(request, ErrorCodes::ActionNotImplemented().code(),
                     Literals::HttpMethodNotImplemented(), out);
  return false;
}

// static
bool WriteResponse::HttpErrorResponse(EHttpStatusCode status_code,
                                      const Printable& body, Print& out) {
  TAS_CHECK_GE(status_code, EHttpStatusCode::kHttpBadRequest)
      << TASLIT("Status code should be for an error.");
  if (status_code < EHttpStatusCode::kHttpBadRequest) {
    status_code = EHttpStatusCode::kHttpInternalServerError;
  }

  HttpResponseHeader hrh;
  hrh.status_code = status_code;

  switch (status_code) {
    case EHttpStatusCode::kHttpBadRequest:
      hrh.reason_phrase = Literals::HttpBadRequest();
      break;
    case EHttpStatusCode::kHttpNotFound:
      hrh.reason_phrase = Literals::HttpNotFound();
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
    default:
      if (status_code < EHttpStatusCode::kHttpBadRequest) {
        hrh.status_code = EHttpStatusCode::kHttpInternalServerError;
      }
      TAS_FALLTHROUGH_INTENDED;
    case EHttpStatusCode::kHttpInternalServerError:
      hrh.reason_phrase = Literals::HttpInternalServerError();
      break;
  }

  hrh.content_type = EContentType::kTextPlain;
  hrh.content_length = CountingBitbucket::SizeOfPrintable(body);
  hrh.do_close = true;
  hrh.printTo(out);
  body.printTo(out);
  return false;
}
}  // namespace alpaca

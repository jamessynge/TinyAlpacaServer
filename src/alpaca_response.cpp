#include "alpaca_response.h"

#include "ascom_error_codes.h"
#include "http_response_header.h"
#include "json_response.h"
#include "literals.h"
#include "utils/json_encoder.h"
#include "utils/platform.h"

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

bool WriteOkResponse(const JsonPropertySource& source, EHttpMethod http_method,
                     Print& out) {
  HttpResponseHeader hrh;
  hrh.status_code = EHttpStatusCode::kHttpOk;
  hrh.reason_phrase = Literals::OK();
  hrh.content_type = EContentType::kApplicationJson;
  hrh.content_length = JsonObjectEncoder::EncodedSize(source);
  hrh.printTo(out);
  if (http_method != EHttpMethod::HEAD) {
    JsonObjectEncoder::Encode(source, out);
  }
  return true;
}

bool WriteArrayResponse(const AlpacaRequest& request,
                        const JsonElementSource& value, Print& out) {
  JsonArrayResponse source(request, value);
  return WriteOkResponse(source, request.http_method, out);
}

bool WriteBoolResponse(const AlpacaRequest& request, bool value, Print& out) {
  JsonBoolResponse source(request, value);
  return WriteOkResponse(source, request.http_method, out);
}

bool WriteDoubleResponse(const AlpacaRequest& request, double value,
                         Print& out) {
  JsonDoubleResponse source(request, value);
  return WriteOkResponse(source, request.http_method, out);
}
bool WriteDoubleResponse(const AlpacaRequest& request,
                         StatusOr<double> status_or_value, Print& out) {
  if (status_or_value.ok()) {
    return WriteDoubleResponse(request, status_or_value.value(), out);
  } else {
    return WriteAscomErrorResponse(request, status_or_value.status(), out);
  }
}

bool WriteFloatResponse(const AlpacaRequest& request, float value, Print& out) {
  JsonFloatResponse source(request, value);
  return WriteOkResponse(source, request.http_method, out);
}

bool WriteIntegerResponse(const AlpacaRequest& request, uint32_t value,
                          Print& out) {
  JsonUnsignedIntegerResponse source(request, value);
  return WriteOkResponse(source, request.http_method, out);
}

bool WriteIntegerResponse(const AlpacaRequest& request, int32_t value,
                          Print& out) {
  JsonIntegerResponse source(request, value);
  return WriteOkResponse(source, request.http_method, out);
}

bool WriteLiteralArrayResponse(const AlpacaRequest& request,
                               const LiteralArray& value, Print& out) {
  WriteArrayResponse(request, LiteralArraySource(value), out);
  return true;
}

bool WriteStringResponse(const AlpacaRequest& request, AnyString value,
                         Print& out) {
  JsonStringResponse source(request, value);
  return WriteOkResponse(source, request.http_method, out);
}

bool WriteStringResponse(const AlpacaRequest& request, Printable& value,
                         Print& out) {
  JsonStringResponse source(request, value);
  return WriteOkResponse(source, request.http_method, out);
}

bool WriteAscomErrorResponse(const AlpacaRequest& request,
                             uint32_t error_number, AnyString error_message,
                             Print& out) {
  JsonMethodResponse source(request, error_number, error_message);
  return WriteOkResponse(source, request.http_method, out);
}

bool WriteAscomErrorResponse(const AlpacaRequest& request,
                             uint32_t error_number, Printable& error_message,
                             Print& out) {
  JsonMethodResponse source(request, error_number, error_message);
  return WriteOkResponse(source, request.http_method, out);
}

bool WriteAscomErrorResponse(const AlpacaRequest& request, Status error_status,
                             Print& out) {
  AnyString error_message;  // TODO(jamessynge): Come up with a way for Status
                            // to carry a message.
  JsonMethodResponse source(request, error_status.code(), error_message);
  return WriteOkResponse(source, request.http_method, out);
}

bool WriteAscomNotImplementedErrorResponse(const AlpacaRequest& request,
                                           Print& out) {
  return WriteAscomErrorResponse(request,
                                 ErrorCodes::ActionNotImplemented().code(),
                                 Literals::HttpMethodNotImplemented(), out);
}

bool WriteHttpErrorResponse(EHttpStatusCode status_code, AnyString& body,
                            Print& out) {
  TAS_DCHECK_GE(status_code, EHttpStatusCode::kHttpBadRequest,
                "Status code should be for an error.");
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
    case EHttpStatusCode::kHttpInternalServerError:
    // ABSL_FALLTHROUGH_INTENDED
    default:
      hrh.status_code = EHttpStatusCode::kHttpInternalServerError;
      hrh.reason_phrase = Literals::HttpInternalServerError();
      break;
  }

  hrh.content_type = EContentType::kTextPlain;
  hrh.content_length = body.size();
  hrh.printTo(out);
  body.printTo(out);
  return false;
}
}  // namespace alpaca

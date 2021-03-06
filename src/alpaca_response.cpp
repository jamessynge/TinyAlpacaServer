#include "alpaca_response.h"

#include "http_response_header.h"
#include "json_encoder.h"
#include "json_response.h"
#include "platform.h"

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

void WriteOkResponse(const JsonPropertySource& source, Print& out) {
  HttpResponseHeader hrh;
  hrh.status_code = EHttpStatusCode::kHttpOk;
  hrh.reason_phrase = Literals::OK();
  hrh.content_type = EContentType::kApplicationJson;
  hrh.content_length = JsonObjectEncoder::EncodedSize(source);
  hrh.printTo(out);
  JsonObjectEncoder::Encode(source, out);
}

void WriteArrayResponse(const AlpacaRequest& request,
                        const JsonElementSource& value, Print& out) {
  JsonArrayResponse source(request, value);
  WriteOkResponse(source, out);
}

void WriteBoolResponse(const AlpacaRequest& request, bool value, Print& out) {
  JsonBoolResponse source(request, value);
  WriteOkResponse(source, out);
}

void WriteDoubleResponse(const AlpacaRequest& request, double value,
                         Print& out) {
  JsonDoubleResponse source(request, value);
  WriteOkResponse(source, out);
}

void WriteFloatResponse(const AlpacaRequest& request, float value, Print& out) {
  JsonFloatResponse source(request, value);
  WriteOkResponse(source, out);
}

void WriteIntegerResponse(const AlpacaRequest& request, uint32_t value,
                          Print& out) {
  JsonUnsignedIntegerResponse source(request, value);
  WriteOkResponse(source, out);
}

void WriteIntegerResponse(const AlpacaRequest& request, int32_t value,
                          Print& out) {
  JsonIntegerResponse source(request, value);
  WriteOkResponse(source, out);
}

void WriteLiteralArrayResponse(const AlpacaRequest& request,
                               const LiteralArray& value, Print& out) {
  WriteArrayResponse(request, LiteralArraySource(value), out);
}

void WriteStringResponse(const AlpacaRequest& request, const AnyString& value,
                         Print& out) {
  JsonStringResponse source(request, value);
  WriteOkResponse(source, out);
}

}  // namespace alpaca

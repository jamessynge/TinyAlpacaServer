#include "src/json_encoder.h"

// Author: james.synge@gmail.com

#include "src/platform.h"
#include "src/string_view.h"

namespace alpaca {
namespace {

// Writes a string literal to out.
//
// NOTE: The length of a literal string includes the NUL (\0) at the end, so we
// subtract one from N to get the length of the string before that.
template <size_t N>
inline void PrintStringLiteral(Print& out, const char (&buf)[N]) {
  out.write(buf, N - 1);
}

void PrintBoolean(Print& out, const bool value) {
  if (value) {
    PrintStringLiteral(out, "true");
  } else {
    PrintStringLiteral(out, "false");
  }
}

template <typename T>
void PrintInteger(Print& out, const T value) {
  // +0 to cause promotion of chars, if they're passed in.
  out.print(value + static_cast<uint16_t>(0));
}

// Prints the floating point value to out, if possible. If not, prints a JSON
// string that "describes" the situation. This is similar to various JSON
// libraries, on which this is based.
template <typename T>
void PrintFloatingPoint(Print& out, const T value) {
  // We're assuming that the stream is configured to match JSON requirements for
  // the formatting of numbers.
#if TAS_HOST_TARGET
  if (std::isnan(value)) {
    JsonStringView("NaN").printTo(out);
  } else if (!std::isfinite(value)) {
    StringView v("-Inf");
    if (value > 0) {
      v.remove_prefix(1);
    }
    v.escaped().printTo(out);
  } else {
#endif
    out.print(value);
#if TAS_HOST_TARGET
  }
#endif
}

#if TAS_HOST_TARGET
class ElementSourceFunctionAdapter : public JsonElementSource {
 public:
  explicit ElementSourceFunctionAdapter(const JsonElementSourceFunction& func)
      : func_(func) {}
  void AddTo(JsonArrayEncoder& encoder) override { func_(encoder); }

 private:
  const JsonElementSourceFunction& func_;
};

class PropertySourceFunctionAdapter : public JsonPropertySource {
 public:
  explicit PropertySourceFunctionAdapter(const JsonPropertySourceFunction& func)
      : func_(func) {}
  void AddTo(JsonObjectEncoder& encoder) override { func_(encoder); }

 private:
  const JsonPropertySourceFunction& func_;
};
#endif  // TAS_HOST_TARGET

}  // namespace

JsonElementSource::~JsonElementSource() {}

JsonPropertySource::~JsonPropertySource() {}

AbstractJsonEncoder::AbstractJsonEncoder(Print& out)
    : out_(out), first_(true) {}

void AbstractJsonEncoder::StartItem() {
  if (first_) {
    first_ = false;
  } else {
    PrintStringLiteral(out_, ", ");
  }
}

void AbstractJsonEncoder::EncodeChildArray(JsonElementSource& source) {
  JsonArrayEncoder encoder(out_);
  source.AddTo(encoder);
}

void AbstractJsonEncoder::EncodeChildObject(JsonPropertySource& source) {
  JsonObjectEncoder encoder(out_);
  source.AddTo(encoder);
}

////////////////////////////////////////////////////////////////////////////////

JsonArrayEncoder::JsonArrayEncoder(Print& out) : AbstractJsonEncoder(out) {
  PrintStringLiteral(out_, "[");
}

JsonArrayEncoder::~JsonArrayEncoder() { PrintStringLiteral(out_, "]"); }

void JsonArrayEncoder::AddIntegerElement(const int32_t value) {
  StartItem();
  PrintInteger(out_, value);
}

void JsonArrayEncoder::AddIntegerElement(const uint32_t value) {
  StartItem();
  PrintInteger(out_, value);
}

void JsonArrayEncoder::AddFloatingPointElement(float value) {
  StartItem();
  PrintFloatingPoint(out_, value);
}

void JsonArrayEncoder::AddFloatingPointElement(double value) {
  StartItem();
  PrintFloatingPoint(out_, value);
}

void JsonArrayEncoder::AddBooleanElement(const bool value) {
  StartItem();
  PrintBoolean(out_, value);
}

void JsonArrayEncoder::AddStringElement(const StringView& value) {
  StartItem();
  value.escaped().printTo(out_);
}

void JsonArrayEncoder::AddArrayElement(JsonElementSource& source) {
  StartItem();
  EncodeChildArray(source);
}

void JsonArrayEncoder::AddObjectElement(JsonPropertySource& source) {
  StartItem();
  EncodeChildObject(source);
}

// static
void JsonArrayEncoder::Encode(JsonElementSource& source, Print& out) {
  JsonArrayEncoder encoder(out);
  source.AddTo(encoder);
}

#if TAS_HOST_TARGET
// static
void JsonArrayEncoder::Encode(const JsonElementSourceFunction& func,
                              Print& out) {
  ElementSourceFunctionAdapter source(func);
  Encode(source, out);
}

void JsonArrayEncoder::AddArrayElement(const JsonElementSourceFunction& func) {
  ElementSourceFunctionAdapter source(func);
  AddArrayElement(source);
}

void JsonArrayEncoder::AddObjectElement(
    const JsonPropertySourceFunction& func) {
  PropertySourceFunctionAdapter source(func);
  AddObjectElement(source);
}
#endif  // TAS_HOST_TARGET

////////////////////////////////////////////////////////////////////////////////

JsonObjectEncoder::JsonObjectEncoder(Print& out) : AbstractJsonEncoder(out) {
  PrintStringLiteral(out_, "{");
}

JsonObjectEncoder::~JsonObjectEncoder() { PrintStringLiteral(out_, "}"); }

void JsonObjectEncoder::StartProperty(const StringView& name) {
  StartItem();
  name.escaped().printTo(out_);
  PrintStringLiteral(out_, ": ");
}

void JsonObjectEncoder::AddIntegerProperty(const StringView& name,
                                           int32_t value) {
  StartProperty(name);
  PrintInteger(out_, value);
}

void JsonObjectEncoder::AddIntegerProperty(const StringView& name,
                                           uint32_t value) {
  StartProperty(name);
  PrintInteger(out_, value);
}

void JsonObjectEncoder::AddFloatingPointProperty(const StringView& name,
                                                 float value) {
  StartProperty(name);
  PrintFloatingPoint(out_, value);
}

void JsonObjectEncoder::AddFloatingPointProperty(const StringView& name,
                                                 double value) {
  StartProperty(name);
  PrintFloatingPoint(out_, value);
}

void JsonObjectEncoder::AddBooleanProperty(const StringView& name,
                                           const bool value) {
  StartProperty(name);
  PrintBoolean(out_, value);
}

void JsonObjectEncoder::AddStringProperty(const StringView& name,
                                          const StringView& value) {
  StartProperty(name);
  value.escaped().printTo(out_);
}
void JsonObjectEncoder::AddArrayProperty(const StringView& name,
                                         JsonElementSource& source) {
  StartProperty(name);
  EncodeChildArray(source);
}

void JsonObjectEncoder::AddObjectProperty(const StringView& name,
                                          JsonPropertySource& source) {
  StartProperty(name);
  EncodeChildObject(source);
}

// static
void JsonObjectEncoder::Encode(JsonPropertySource& source, Print& out) {
  JsonObjectEncoder encoder(out);
  source.AddTo(encoder);
}

#if TAS_HOST_TARGET
// static
void JsonObjectEncoder::Encode(const JsonPropertySourceFunction& func,
                               Print& out) {
  PropertySourceFunctionAdapter source(func);
  Encode(source, out);
}

void JsonObjectEncoder::AddArrayProperty(
    const StringView& name, const JsonElementSourceFunction& func) {
  ElementSourceFunctionAdapter source(func);
  AddArrayProperty(name, source);
}

void JsonObjectEncoder::AddObjectProperty(
    const StringView& name, const JsonPropertySourceFunction& func) {
  PropertySourceFunctionAdapter source(func);
  AddObjectProperty(name, source);
}
#endif  // TAS_HOST_TARGET

}  // namespace alpaca

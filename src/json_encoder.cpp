#include "json_encoder.h"

// Author: james.synge@gmail.com

#include "literals.h"
#include "platform.h"
#include "string_view.h"

namespace alpaca {
namespace {

void PrintBoolean(Print& out, const bool value) {
  if (value) {
    Literals::JsonTrue().printTo(out);
  } else {
    Literals::JsonFalse().printTo(out);
  }
}

template <typename T>
void PrintInteger(Print& out, const T value) {
  // +0 to cause promotion of chars, if they're passed in.
  out.print(value + static_cast<uint16_t>(0));
}

// Prints the floating point value to out, if possible. If not, prints a JSON
// string that "describes" the situation. This is similar to various JSON
// libraries, which inspired this. TBD whether this is a good idea in the ASCOM
// Alpaca setting.
template <typename T>
void PrintFloatingPoint(Print& out, const T value) {
#if TAS_HOST_TARGET
  // Haven't got std::isnan or std::isfinite in the Arduino environment.
  // TODO(jamessynge): Consider using isnan and isfinite from avr-libc's math.h.
  if (std::isnan(value)) {
    Literals::JsonNan().printJsonEscapedTo(out);
  } else if (!std::isfinite(value)) {
    if (value > 0) {
      Literals::JsonInf().printJsonEscapedTo(out);
    } else {
      Literals::JsonNegInf().printJsonEscapedTo(out);
    }
  } else {
#endif
    // We're assuming that the Print object is configured to match JSON
    // requirements for the formatting of numbers.
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

AbstractJsonEncoder::EncodeableString::EncodeableString(const Literal& literal)
    : literal_(literal), is_literal_(true) {}

AbstractJsonEncoder::EncodeableString::EncodeableString(const StringView& view)
    : view_(view), is_literal_(false) {}

size_t AbstractJsonEncoder::EncodeableString::printTo(Print& out) const {
  if (is_literal_) {
    return literal_.printTo(out);
  } else {
    return view_.printTo(out);
  }
}
size_t AbstractJsonEncoder::EncodeableString::printJsonEscapedTo(
    Print& out) const {
  if (is_literal_) {
    return literal_.printJsonEscapedTo(out);
  } else {
    return view_.escaped().printTo(out);
  }
}

AbstractJsonEncoder::AbstractJsonEncoder(Print& out)
    : out_(out), first_(true) {}

void AbstractJsonEncoder::StartItem() {
  if (first_) {
    first_ = false;
  } else {
    out_.print(',');
    out_.print(' ');
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
  out_.print('[');
}

JsonArrayEncoder::~JsonArrayEncoder() { out_.print(']'); }

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

void JsonArrayEncoder::AddStringElement(const EncodeableString& value) {
  StartItem();
  value.printJsonEscapedTo(out_);
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
  out_.print('{');
}

JsonObjectEncoder::~JsonObjectEncoder() { out_.print('}'); }

void JsonObjectEncoder::StartProperty(const EncodeableString& name) {
  StartItem();
  name.printJsonEscapedTo(out_);
  out_.print(':');
  out_.print(' ');
}

void JsonObjectEncoder::AddIntegerProperty(const EncodeableString& name,
                                           int32_t value) {
  StartProperty(name);
  PrintInteger(out_, value);
}

void JsonObjectEncoder::AddIntegerProperty(const EncodeableString& name,
                                           uint32_t value) {
  StartProperty(name);
  PrintInteger(out_, value);
}

void JsonObjectEncoder::AddFloatingPointProperty(const EncodeableString& name,
                                                 float value) {
  StartProperty(name);
  PrintFloatingPoint(out_, value);
}

void JsonObjectEncoder::AddFloatingPointProperty(const EncodeableString& name,
                                                 double value) {
  StartProperty(name);
  PrintFloatingPoint(out_, value);
}

void JsonObjectEncoder::AddBooleanProperty(const EncodeableString& name,
                                           const bool value) {
  StartProperty(name);
  PrintBoolean(out_, value);
}

void JsonObjectEncoder::AddStringProperty(const EncodeableString& name,
                                          const EncodeableString& value) {
  StartProperty(name);
  value.printJsonEscapedTo(out_);
}

void JsonObjectEncoder::AddArrayProperty(const EncodeableString& name,
                                         JsonElementSource& source) {
  StartProperty(name);
  EncodeChildArray(source);
}

void JsonObjectEncoder::AddObjectProperty(const EncodeableString& name,
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
    const EncodeableString& name, const JsonElementSourceFunction& func) {
  ElementSourceFunctionAdapter source(func);
  AddArrayProperty(name, source);
}

void JsonObjectEncoder::AddObjectProperty(
    const EncodeableString& name, const JsonPropertySourceFunction& func) {
  PropertySourceFunctionAdapter source(func);
  AddObjectProperty(name, source);
}
#endif  // TAS_HOST_TARGET

}  // namespace alpaca

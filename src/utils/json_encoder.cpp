#include "utils/json_encoder.h"

// Author: james.synge@gmail.com

#include "utils/counting_bitbucket.h"
#include "utils/literal.h"
#include "utils/platform.h"

namespace alpaca {
namespace {

TAS_DEFINE_LITERAL(JsonTrue, "true")
TAS_DEFINE_LITERAL(JsonFalse, "false")
TAS_DEFINE_LITERAL(JsonNan, "NaN")
TAS_DEFINE_LITERAL(JsonNegInf, "-Inf")
TAS_DEFINE_LITERAL(JsonInf, "Inf")

void PrintBoolean(Print& out, const bool value) {
  if (value) {
    JsonTrue().printTo(out);
  } else {
    JsonFalse().printTo(out);
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
    JsonNan().printJsonEscapedTo(out);
  } else if (!std::isfinite(value)) {
    if (value > 0) {
      JsonInf().printJsonEscapedTo(out);
    } else {
      JsonNegInf().printJsonEscapedTo(out);
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

}  // namespace

JsonElementSource::~JsonElementSource() {}

JsonPropertySource::~JsonPropertySource() {}

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

void AbstractJsonEncoder::EncodeChildArray(const JsonElementSource& source) {
  JsonArrayEncoder encoder(out_);
  source.AddTo(encoder);
}

void AbstractJsonEncoder::EncodeChildObject(const JsonPropertySource& source) {
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

void JsonArrayEncoder::AddStringElement(const AnyString& value) {
  StartItem();
  value.printJsonEscapedTo(out_);
}

void JsonArrayEncoder::AddArrayElement(const JsonElementSource& source) {
  StartItem();
  EncodeChildArray(source);
}

void JsonArrayEncoder::AddObjectElement(const JsonPropertySource& source) {
  StartItem();
  EncodeChildObject(source);
}

// static
void JsonArrayEncoder::Encode(const JsonElementSource& source, Print& out) {
  JsonArrayEncoder encoder(out);
  source.AddTo(encoder);
}

// static
size_t JsonArrayEncoder::EncodedSize(const JsonElementSource& source) {
  CountingBitbucket counter;
  Encode(source, counter);
  return counter.count();
}

////////////////////////////////////////////////////////////////////////////////

JsonObjectEncoder::JsonObjectEncoder(Print& out) : AbstractJsonEncoder(out) {
  out_.print('{');
}

JsonObjectEncoder::~JsonObjectEncoder() { out_.print('}'); }

void JsonObjectEncoder::StartProperty(const AnyString& name) {
  StartItem();
  name.printJsonEscapedTo(out_);
  out_.print(':');
  out_.print(' ');
}

void JsonObjectEncoder::AddIntegerProperty(const AnyString& name,
                                           int32_t value) {
  StartProperty(name);
  PrintInteger(out_, value);
}

void JsonObjectEncoder::AddIntegerProperty(const AnyString& name,
                                           uint32_t value) {
  StartProperty(name);
  PrintInteger(out_, value);
}

void JsonObjectEncoder::AddFloatingPointProperty(const AnyString& name,
                                                 float value) {
  StartProperty(name);
  PrintFloatingPoint(out_, value);
}

void JsonObjectEncoder::AddFloatingPointProperty(const AnyString& name,
                                                 double value) {
  StartProperty(name);
  PrintFloatingPoint(out_, value);
}

void JsonObjectEncoder::AddBooleanProperty(const AnyString& name,
                                           const bool value) {
  StartProperty(name);
  PrintBoolean(out_, value);
}

void JsonObjectEncoder::AddStringProperty(const AnyString& name,
                                          const AnyString& value) {
  StartProperty(name);
  value.printJsonEscapedTo(out_);
}

void JsonObjectEncoder::AddArrayProperty(const AnyString& name,
                                         const JsonElementSource& source) {
  StartProperty(name);
  EncodeChildArray(source);
}

void JsonObjectEncoder::AddObjectProperty(const AnyString& name,
                                          const JsonPropertySource& source) {
  StartProperty(name);
  EncodeChildObject(source);
}

// static
void JsonObjectEncoder::Encode(const JsonPropertySource& source, Print& out) {
  JsonObjectEncoder encoder(out);
  source.AddTo(encoder);
}

// static
size_t JsonObjectEncoder::EncodedSize(const JsonPropertySource& source) {
  CountingBitbucket counter;
  Encode(source, counter);
  return counter.count();
}

}  // namespace alpaca

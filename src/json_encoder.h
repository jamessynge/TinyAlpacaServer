#ifndef TINY_ALPACA_SERVER_JSON_ENCODER_H_
#define TINY_ALPACA_SERVER_JSON_ENCODER_H_

// Supports writing a JSON object, with values that are numbers, bools, strings,
// and arrays of the same. Usage examples can be // found in the test file.
//
// Author: james.synge@gmail.com

#include "platform.h"
#include "string_view.h"

namespace alpaca {
// namespace internal {

// // Writes a string literal to out.
// // NOTE: The length of a literal string includes the NUL (\0) at the end, so
// we
// // subtract one from N to get the length of the string before that.

// template <size_t N>
// inline void PrintStringLiteral(Print& out, const char (&buf)[N]) {
//   out.write(buf, N - 1);
// }

// template <typename T>
// void PrintInteger(Print& out, const T value) {
//   // +0 to cause promotion of chars, if they're passed in.
//   out.print(value + static_cast<uint16_t>(0));
// }

// // Prints the floating point value to out, if possible. If not, prints a JSON
// // string that "describes" the situation. This is similar to various JSON
// // libraries, on which this is based.
// template <typename T>
// void PrintFloatingPoint(Print& out, const T value) {
//   // We're assuming that the stream is configured to match JSON requirements
//   for
//   // the formatting of numbers.
// #if TAS_HOST_TARGET
//   if (std::isnan(value)) {
//     JsonStringView("NaN").printTo(out);
//   } else if (!std::isfinite(value)) {
//     StringView v("-Inf");
//     if (value > 0) {
//       v.remove_prefix(1);
//     }
//     v.escaped().printTo(out);
//   } else {
// #endif
//     out.print(value);
// #if TAS_HOST_TARGET
//   }
// #endif
// }

// // Prints true or false to out.
// void PrintBoolean(Print& out, const bool value);

// }  // namespace internal

class JsonArrayEncoder;
class JsonObjectEncoder;

class JsonElementSource {
 public:
  virtual ~JsonElementSource();
  virtual void AddTo(JsonArrayEncoder& encoder) = 0;
};

class JsonPropertySource {
 public:
  virtual ~JsonPropertySource();
  virtual void AddTo(JsonObjectEncoder& object_encoder) = 0;
};

// Base class for the object and array encoders.
class AbstractJsonEncoder {
 protected:
  explicit AbstractJsonEncoder(Print& out);

  AbstractJsonEncoder(const AbstractJsonEncoder&) = delete;
  AbstractJsonEncoder(AbstractJsonEncoder&&) = delete;
  AbstractJsonEncoder& operator=(const AbstractJsonEncoder&) = delete;
  AbstractJsonEncoder& operator=(AbstractJsonEncoder&&) = delete;

  // Prints the comma between elements in an array or properties in an object.
  void StartItem();

  void EncodeChildArray(JsonElementSource& source);
  void EncodeChildObject(JsonPropertySource& source);

  Print& out_;

 private:
  bool first_;
};

// JSON encoder for arrays.
class JsonArrayEncoder : public AbstractJsonEncoder {
 public:
  static void Encode(JsonElementSource& source, Print& out);

  void AddIntegerElement(const int32_t value);
  void AddIntegerElement(const uint32_t value);
  void AddFloatingPointElement(float value);
  void AddFloatingPointElement(double value);
  void AddBooleanElement(const bool value);
  void AddStringElement(const StringView& value);
  void AddArrayElement(JsonElementSource& source);
  void AddObjectElement(JsonPropertySource& source);

 private:
  friend class AbstractJsonEncoder;

  explicit JsonArrayEncoder(Print& out);
  JsonArrayEncoder(const JsonArrayEncoder&) = delete;
  JsonArrayEncoder(JsonArrayEncoder&&) = delete;
  JsonArrayEncoder& operator=(const JsonArrayEncoder&) = delete;
  JsonArrayEncoder& operator=(JsonArrayEncoder&&) = delete;

  ~JsonArrayEncoder();
};

// JSON encoder for objects.
class JsonObjectEncoder : public AbstractJsonEncoder {
 public:
  static void Encode(JsonPropertySource& source, Print& out);

  void AddIntegerProperty(const StringView& name, int32_t value);
  void AddIntegerProperty(const StringView& name, uint32_t value);
  void AddFloatingPointProperty(const StringView& name, float value);
  void AddFloatingPointProperty(const StringView& name, double value);
  void AddBooleanProperty(const StringView& name, const bool value);
  void AddStringProperty(const StringView& name, const StringView& value);
  void AddArrayProperty(const StringView& name, JsonElementSource& source);
  void AddObjectProperty(const StringView& name, JsonPropertySource& source);

 private:
  friend class AbstractJsonEncoder;

  explicit JsonObjectEncoder(Print& out);
  JsonObjectEncoder(const JsonObjectEncoder&) = delete;
  JsonObjectEncoder(JsonObjectEncoder&&) = delete;
  JsonObjectEncoder& operator=(const JsonObjectEncoder&) = delete;
  JsonObjectEncoder& operator=(JsonObjectEncoder&&) = delete;

  ~JsonObjectEncoder();

  void StartProperty(const StringView& name);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_JSON_ENCODER_H_

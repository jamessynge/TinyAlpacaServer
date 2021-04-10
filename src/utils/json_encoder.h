#ifndef TINY_ALPACA_SERVER_SRC_UTILS_JSON_ENCODER_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_JSON_ENCODER_H_

// Supports writing a JSON object, with values that are numbers, bools, strings,
// and arrays of the same. Usage examples can be found in the test file.
//
// Author: james.synge@gmail.com

#include "utils/any_printable.h"
#include "utils/array_view.h"
#include "utils/platform.h"

namespace alpaca {

class JsonArrayEncoder;
class JsonObjectEncoder;

class JsonElementSource {
 public:
  virtual ~JsonElementSource();
  virtual void AddTo(JsonArrayEncoder& encoder) const = 0;
};

class JsonPropertySource {
 public:
  virtual ~JsonPropertySource();
  virtual void AddTo(JsonObjectEncoder& encoder) const = 0;
};

// IDEA: Add a JsonEncoderOptions struct, similar to the kwargs for Python's
// json.dump function (i.e. to support the amount and kind of whitespace between
// items, including support for indenting or omitting all whitespace).

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

  void EncodeChildArray(const JsonElementSource& source);
  void EncodeChildObject(const JsonPropertySource& source);

  void PrintString(const Printable& printable);

  Print& out_;

 private:
  bool first_;
};

// JSON encoder for arrays.
class JsonArrayEncoder : public AbstractJsonEncoder {
 public:
  void AddIntElement(const int32_t value);
  void AddUIntElement(const uint32_t value);
  void AddFloatElement(float value);
  void AddDoubleElement(double value);
  void AddBooleanElement(const bool value);
  void AddStringElement(const AnyPrintable& value);
  void AddStringElement(const Printable& value);
  void AddArrayElement(const JsonElementSource& source);
  void AddObjectElement(const JsonPropertySource& source);

  static void Encode(const JsonElementSource& source, Print& out);
  static size_t EncodedSize(const JsonElementSource& source);

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
  void AddIntProperty(const AnyPrintable& name, int32_t value);
  void AddUIntProperty(const AnyPrintable& name, uint32_t value);
  void AddFloatProperty(const AnyPrintable& name, float value);
  void AddDoubleProperty(const AnyPrintable& name, double value);
  void AddBooleanProperty(const AnyPrintable& name, const bool value);
  void AddStringProperty(const AnyPrintable& name, const AnyPrintable& value);
  void AddStringProperty(const AnyPrintable& name, const Printable& value);
  void AddArrayProperty(const AnyPrintable& name,
                        const JsonElementSource& source);
  void AddObjectProperty(const AnyPrintable& name,
                         const JsonPropertySource& source);

  static void Encode(const JsonPropertySource& source, Print& out);
  static size_t EncodedSize(const JsonPropertySource& source);

 private:
  friend class AbstractJsonEncoder;

  explicit JsonObjectEncoder(Print& out);
  JsonObjectEncoder(const JsonObjectEncoder&) = delete;
  JsonObjectEncoder(JsonObjectEncoder&&) = delete;
  JsonObjectEncoder& operator=(const JsonObjectEncoder&) = delete;
  JsonObjectEncoder& operator=(JsonObjectEncoder&&) = delete;

  ~JsonObjectEncoder();

  void StartProperty(const AnyPrintable& name);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_JSON_ENCODER_H_

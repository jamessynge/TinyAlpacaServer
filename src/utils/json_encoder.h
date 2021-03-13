#ifndef TINY_ALPACA_SERVER_SRC_UTILS_JSON_ENCODER_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_JSON_ENCODER_H_

// Supports writing a JSON object, with values that are numbers, bools, strings,
// and arrays of the same. Usage examples can be // found in the test file.
//
// Author: james.synge@gmail.com

// TODO(jamessynge): Study
// http://p-nand-q.com/programming/cplusplus/duck_typing_and_templates.html to
// determine if it can make it easier to implement the methods below that take
// an AnyString or a Printable&. We just want to accept any X that has a |size_t
// X::printTo(Print&) const| method, or for which there is a |size_t
// Print::print(X)| method.

#include "utils/any_string.h"
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

// JsonElementSourceAdapter allows an object that can't have a virtual function
// table (e.g. it is constexpr and maybe stored in PROGMEM) to act as a
// JsonElementSource.
template <class T>
class JsonElementSourceAdapter : public JsonElementSource {
 public:
  explicit JsonElementSourceAdapter(const T& wrapped) : wrapped_(wrapped) {}
  void AddTo(JsonArrayEncoder& encoder) const override {
    wrapped_.AddTo(encoder);
  }

 private:
  const T& wrapped_;
};

// JsonPropertySourceAdapter allows an object that can't have a virtual function
// table (e.g. it is constexpr and maybe stored in PROGMEM) to act as a
// JsonPropertySource.
template <class T>
class JsonPropertySourceAdapter : public JsonPropertySource {
 public:
  explicit JsonPropertySourceAdapter(const T& wrapped) : wrapped_(wrapped) {}
  void AddTo(JsonObjectEncoder& encoder) const override {
    wrapped_.AddTo(encoder);
  }

 private:
  const T& wrapped_;
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

  void EncodeChildArray(const JsonElementSource& source);
  void EncodeChildObject(const JsonPropertySource& source);

  Print& out_;

 private:
  bool first_;
};

// JSON encoder for arrays.
class JsonArrayEncoder : public AbstractJsonEncoder {
 public:
  void AddIntegerElement(const int32_t value);
  void AddIntegerElement(const uint32_t value);
  void AddFloatingPointElement(float value);
  void AddFloatingPointElement(double value);
  void AddBooleanElement(const bool value);
  void AddStringElement(AnyString value);
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
  void AddIntegerProperty(const AnyString& name, int32_t value);
  void AddIntegerProperty(const AnyString& name, uint32_t value);
  void AddFloatingPointProperty(const AnyString& name, float value);
  void AddFloatingPointProperty(const AnyString& name, double value);
  void AddBooleanProperty(const AnyString& name, const bool value);
  void AddStringProperty(const AnyString& name, AnyString value);
  void AddStringProperty(const AnyString& name, const Printable& value);
  void AddArrayProperty(const AnyString& name, const JsonElementSource& source);
  void AddObjectProperty(const AnyString& name,
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

  void StartProperty(const AnyString& name);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_JSON_ENCODER_H_

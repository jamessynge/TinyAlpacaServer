#ifndef TINY_ALPACA_SERVER_SRC_JSON_ENCODER_H_
#define TINY_ALPACA_SERVER_SRC_JSON_ENCODER_H_

// Supports writing a JSON object, with values that are numbers, bools, strings,
// and arrays of the same. Usage examples can be // found in the test file.
//
// Author: james.synge@gmail.com

#include "any_string.h"
#include "extras/host_arduino/print.h"
#include "platform.h"

#if TAS_HOST_TARGET
#include <functional>
#endif  // TAS_HOST_TARGET

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

#if TAS_HOST_TARGET
// These support tests.
using JsonElementSourceFunction = std::function<void(JsonArrayEncoder&)>;
using JsonPropertySourceFunction = std::function<void(JsonObjectEncoder&)>;
#endif  // TAS_HOST_TARGET

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
  void AddStringElement(const AnyString& value);
  void AddArrayElement(const JsonElementSource& source);
  void AddObjectElement(const JsonPropertySource& source);

  static void Encode(const JsonElementSource& source, Print& out);

#if TAS_HOST_TARGET
  static void Encode(const JsonElementSourceFunction& func, Print& out);
  void AddArrayElement(const JsonElementSourceFunction& func);
  void AddObjectElement(const JsonPropertySourceFunction& func);
#endif  // TAS_HOST_TARGET

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
  void AddStringProperty(const AnyString& name, const AnyString& value);
  void AddArrayProperty(const AnyString& name, const JsonElementSource& source);
  void AddObjectProperty(const AnyString& name,
                         const JsonPropertySource& source);

  static void Encode(const JsonPropertySource& source, Print& out);

#if TAS_HOST_TARGET
  static void Encode(const JsonPropertySourceFunction& func, Print& out);
  void AddArrayProperty(const AnyString& name,
                        const JsonElementSourceFunction& func);
  void AddObjectProperty(const AnyString& name,
                         const JsonPropertySourceFunction& func);
#endif  // TAS_HOST_TARGET

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

#endif  // TINY_ALPACA_SERVER_SRC_JSON_ENCODER_H_

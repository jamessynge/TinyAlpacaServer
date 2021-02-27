#ifndef TINY_ALPACA_SERVER_SRC_JSON_ENCODER_H_
#define TINY_ALPACA_SERVER_SRC_JSON_ENCODER_H_

// Supports writing a JSON object, with values that are numbers, bools, strings,
// and arrays of the same. Usage examples can be // found in the test file.
//
// Author: james.synge@gmail.com

#include "literal.h"
#include "platform.h"
#include "string_view.h"

#if TAS_HOST_TARGET
#include <functional>
#endif  // TAS_HOST_TARGET

namespace alpaca {

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

#if TAS_HOST_TARGET
// These support tests.
using JsonElementSourceFunction = std::function<void(JsonArrayEncoder&)>;
using JsonPropertySourceFunction = std::function<void(JsonObjectEncoder&)>;
#endif  // TAS_HOST_TARGET

// Base class for the object and array encoders.
class AbstractJsonEncoder {
 protected:
  // This is essentially a std::variant<Literal, StringView>. It avoids the need
  // to define two methods where a parameter is some kind of string.
  class EncodeableString {
   public:
    // Deliberately NOT explicit.
    EncodeableString(const Literal& literal);  // NOLINT
    EncodeableString(const StringView& view);  // NOLINT
#if TAS_HOST_TARGET
    // In support of tests, construct a StringView variant.
    template <StringView::size_type N>
    constexpr EncodeableString(const char (&buf)[N])  // NOLINT
        : EncodeableString(StringView(buf)) {}
#endif  // TAS_HOST_TARGET

    size_t printTo(Print& out) const;
    size_t printJsonEscapedTo(Print& out) const;

   private:
    union {
      const Literal literal_;
      const StringView view_;
    };
    const bool is_literal_;
  };

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
  void AddIntegerElement(const int32_t value);
  void AddIntegerElement(const uint32_t value);
  void AddFloatingPointElement(float value);
  void AddFloatingPointElement(double value);
  void AddBooleanElement(const bool value);
  void AddStringElement(const EncodeableString& value);
  void AddArrayElement(JsonElementSource& source);
  void AddObjectElement(JsonPropertySource& source);

  static void Encode(JsonElementSource& source, Print& out);

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
  void AddIntegerProperty(const EncodeableString& name, int32_t value);
  void AddIntegerProperty(const EncodeableString& name, uint32_t value);
  void AddFloatingPointProperty(const EncodeableString& name, float value);
  void AddFloatingPointProperty(const EncodeableString& name, double value);
  void AddBooleanProperty(const EncodeableString& name, const bool value);
  void AddStringProperty(const EncodeableString& name,
                         const EncodeableString& value);
  void AddArrayProperty(const EncodeableString& name,
                        JsonElementSource& source);
  void AddObjectProperty(const EncodeableString& name,
                         JsonPropertySource& source);

  static void Encode(JsonPropertySource& source, Print& out);

#if TAS_HOST_TARGET
  static void Encode(const JsonPropertySourceFunction& func, Print& out);
  void AddArrayProperty(const EncodeableString& name,
                        const JsonElementSourceFunction& func);
  void AddObjectProperty(const EncodeableString& name,
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

  void StartProperty(const EncodeableString& name);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_JSON_ENCODER_H_

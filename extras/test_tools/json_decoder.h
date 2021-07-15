#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_JSON_DECODER_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_JSON_DECODER_H_

// A trivial JSON decoder, intended only to support testing whether the
// responses from Tiny Alpaca Server are correct.

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "absl/status/statusor.h"

namespace alpaca {
namespace test {

class JsonValue;

using JsonObjectBase = std::unordered_map<std::string, JsonValue>;
using JsonArrayBase = std::vector<JsonValue>;

class JsonObject : public JsonObjectBase {
 public:
  using JsonObjectBase::JsonObjectBase;

  template <typename T>
  JsonObject& Add(std::string_view key, T t);
};

class JsonArray : public JsonArrayBase {
 public:
  using JsonArrayBase::JsonArrayBase;

  template <typename T>
  JsonArray& Add(T t);
};

class JsonValue {
 public:
  // This order must match the order of types in the variant value_'s
  // declaration.
  enum EType {
    kNull = 0,
    kBool,
    kNumber,
    kString,
    kObject,
    kArray,
  };

  explicit JsonValue(nullptr_t);
  explicit JsonValue(bool);
  explicit JsonValue(double);
  explicit JsonValue(int);  // NOLINT
  explicit JsonValue(const std::string&);
  explicit JsonValue(std::string_view);
  explicit JsonValue(const char*);
  explicit JsonValue(std::shared_ptr<JsonObject>);
  explicit JsonValue(JsonObject);
  explicit JsonValue(std::shared_ptr<JsonArray>);
  explicit JsonValue(JsonArray);

  // Parse str as a JSON, returning a JsonValue if successful, else an error
  // status.
  static absl::StatusOr<JsonValue> Parse(std::string_view str);

  // Returns the type of value stored.
  EType type() const;

  // The as_X methods will raise an exception if the wrong method is called, so
  // be sure to use type() first.
  bool as_bool() const;
  double as_number() const;
  const std::string& as_string() const;
  const JsonObject& as_object() const;
  const JsonArray& as_array() const;

 private:
  // This order must match the order in EType
  std::variant<void*, bool, double, std::string, std::shared_ptr<JsonObject>,
               std::shared_ptr<JsonArray>>
      value_;
};

template <typename T>
JsonObject& JsonObject::Add(std::string_view key, T t) {
  JsonValue value(t);
  insert_or_assign(std::string(key), value);
  return *this;
}

template <typename T>
JsonArray& JsonArray::Add(T t) {
  JsonValue value(t);
  push_back(value);
  return *this;
}

bool operator==(const JsonValue& jv, nullptr_t);
inline bool operator==(nullptr_t, const JsonValue& jv) { return jv == nullptr; }

bool operator==(const JsonValue& jv, bool b);
inline bool operator==(bool b, const JsonValue& jv) { return jv == b; }

bool operator==(const JsonValue& jv, double d);
inline bool operator==(double d, const JsonValue& jv) { return jv == d; }

bool operator==(const JsonValue& jv, std::string_view s);
inline bool operator==(std::string_view s, const JsonValue& jv) {
  return jv == s;
}

bool operator==(const JsonValue& a, const JsonValue& b);
inline bool operator!=(const JsonValue& a, const JsonValue& b) {
  return !(a == b);
}

template <typename T>
bool operator!=(const JsonValue& a, T b) {
  return !(a == b);
}
template <typename T>
bool operator!=(T a, const JsonValue& b) {
  return !(b == a);
}

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_JSON_DECODER_H_

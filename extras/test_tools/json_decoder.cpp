#include "extras/test_tools/json_decoder.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>  // NOLINT(build/c++11)

#include "absl/status/status.h"
#include "absl/strings/charconv.h"
#include "absl/strings/escaping.h"
#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "logging.h"
#include "util/task/status_macros.h"

namespace alpaca {
namespace test {
namespace {

std::string CharToStr(char c) { return std::string(1, c); }

std::string CharToQuotedStr(char c) { return '\'' + std::string(1, c) + '\''; }

void SkipLeadingWhitespace(std::string_view& str) {
  auto pos = str.find_first_not_of(" \t\r\n");
  if (pos == std::string_view::npos) {
    str.remove_prefix(str.size());
  } else {
    str.remove_prefix(pos);
  }
}

absl::Status SkipInteriorWhitespace(std::string_view& str) {
  SkipLeadingWhitespace(str);
  if (str.empty()) {
    return absl::InvalidArgumentError("Expected a value, not end-of-input");
  } else {
    return absl::OkStatus();
  }
}

absl::Status SkipRequiredDelimiter(std::string_view& str,
                                   const char delimiter) {
  RETURN_IF_ERROR(SkipInteriorWhitespace(str))
      << "Expected delimiter " << CharToQuotedStr(delimiter)
      << ", not end-of-input";
  if (str[0] != delimiter) {
    return absl::InvalidArgumentError(
        absl::StrCat("Expected delimiter ", CharToQuotedStr(delimiter),
                     ", not ", CharToQuotedStr(str[0])));
  }
  str.remove_prefix(1);
  return absl::OkStatus();
}

absl::Status SkipRequiredWord(std::string_view& str,
                              const std::string_view word) {
  RETURN_IF_ERROR(SkipInteriorWhitespace(str))
      << "Expected " << word << ", not end-of-input.";
  if (!absl::StartsWith(str, word)) {
    return absl::InvalidArgumentError(absl::StrCat("Expected ", word));
  }
  str.remove_prefix(word.size());
  return absl::OkStatus();
}

absl::StatusOr<JsonValue> ParseValue(std::string_view& str);

absl::StatusOr<JsonValue> ParseString(std::string_view& str) {
  RETURN_IF_ERROR(SkipRequiredDelimiter(str, '"'));
  std::string result;
  while (!str.empty()) {
    char c = str[0];
    str.remove_prefix(1);
    if (!(' ' <= c && c < 127)) {
      return absl::InvalidArgumentError(
          absl::StrCat("Not a valid string char: ", CharToQuotedStr(c)));
    } else if (c == '"') {
      // End of string.
      return JsonValue(result);
    } else if (c == '\\') {
      if (str.empty()) {
        return absl::InvalidArgumentError(
            "Expected escaped character, not end-of-input");
      }
      c = str[0];
      str.remove_prefix(1);
      switch (c) {
        case '"':
          result.append(1, '"');
          break;
        case '\\':
          result.append(1, '\\');
          break;
        case '/':
          result.append(1, '/');
          break;
        case 'b':
          result.append(1, '\b');
          break;
        case 'f':
          result.append(1, '\f');
          break;
        case 'n':
          result.append(1, '\n');
          break;
        case 'r':
          result.append(1, '\r');
          break;
        case 't':
          result.append(1, '\t');
          break;
        case 'u':
          QCHECK(false) << "Decoding of unicode code points not supported: "
                        << str;
          break;
      }
    } else {
      result.append(1, c);
    }
  }
  return absl::InvalidArgumentError("Expected end of string, not end-of-input");
}

absl::StatusOr<JsonValue> ParseNull(std::string_view& str) {
  RETURN_IF_ERROR(SkipRequiredWord(str, "null"));
  return JsonValue(nullptr);
}

absl::StatusOr<JsonValue> ParseTrue(std::string_view& str) {
  RETURN_IF_ERROR(SkipRequiredWord(str, "true"));
  return JsonValue(true);
}

absl::StatusOr<JsonValue> ParseFalse(std::string_view& str) {
  RETURN_IF_ERROR(SkipRequiredWord(str, "false"));
  return JsonValue(false);
}

absl::StatusOr<JsonValue> ParseNumber(std::string_view& str) {
  RETURN_IF_ERROR(SkipInteriorWhitespace(str))
      << "Expected a number, not end-of-input.";
  double value;
  auto fc_result = absl::from_chars(str.data(), str.data() + str.size(), value);
  if (fc_result.ec != std::errc()) {
    return absl::InvalidArgumentError("Expected a number");
  }
  CHECK_GT(fc_result.ptr, str.data());
  auto size = fc_result.ptr - str.data();
  CHECK_LE(size, str.size());
  str.remove_prefix(size);
  return JsonValue(value);
}

absl::StatusOr<JsonValue> ParseObject(std::string_view& str) {
  RETURN_IF_ERROR(SkipRequiredDelimiter(str, '{'));
  std::shared_ptr<JsonObject> object = std::make_shared<JsonObject>();
  RETURN_IF_ERROR(SkipInteriorWhitespace(str));
  if (str[0] != '}') {
    while (true) {
      ASSIGN_OR_RETURN(auto name, ParseString(str));
      RETURN_IF_ERROR(SkipRequiredDelimiter(str, ':'));
      ASSIGN_OR_RETURN(auto value, ParseValue(str));
      object->insert_or_assign(name.as_string(), value);
      RETURN_IF_ERROR(SkipInteriorWhitespace(str));
      if (str[0] == '}') {
        break;
      }
      RETURN_IF_ERROR(SkipRequiredDelimiter(str, ','));
    }
  }
  RETURN_IF_ERROR(SkipRequiredDelimiter(str, '}'));
  return JsonValue(object);
}

absl::StatusOr<JsonValue> ParseArray(std::string_view& str) {
  RETURN_IF_ERROR(SkipRequiredDelimiter(str, '['));
  std::shared_ptr<JsonArray> array = std::make_shared<JsonArray>();
  RETURN_IF_ERROR(SkipInteriorWhitespace(str));
  if (str[0] != ']') {
    while (true) {
      ASSIGN_OR_RETURN(auto value, ParseValue(str));
      array->push_back(value);
      RETURN_IF_ERROR(SkipInteriorWhitespace(str));
      if (str[0] == ']') {
        break;
      }
      RETURN_IF_ERROR(SkipRequiredDelimiter(str, ','));
    }
  }
  RETURN_IF_ERROR(SkipRequiredDelimiter(str, ']'));
  return JsonValue(array);
}

absl::StatusOr<JsonValue> ParseValue(std::string_view& str) {
  RETURN_IF_ERROR(SkipInteriorWhitespace(str));
  const char c = str[0];
  if (c == '[') {
    return ParseArray(str);
  } else if (c == '{') {
    return ParseObject(str);
  } else if (c == '"') {
    return ParseString(str);
  } else if (c == 'n') {
    return ParseNull(str);
  } else if (c == 't') {
    return ParseTrue(str);
  } else if (c == 'f') {
    return ParseFalse(str);
  } else {
    return ParseNumber(str);
  }
}

}  // namespace

JsonValue::JsonValue() : value_(Undefined{}) {}
JsonValue::JsonValue(nullptr_t) : value_(nullptr) {}
JsonValue::JsonValue(bool v) : value_(v) {}
JsonValue::JsonValue(double v) : value_(v) {}
JsonValue::JsonValue(int v) : value_(static_cast<double>(v)) {}
JsonValue::JsonValue(const std::string& v) : value_(v) {}
JsonValue::JsonValue(std::string_view v) : value_(std::string(v)) {}
JsonValue::JsonValue(const char* v) : value_(std::string(v)) {}
JsonValue::JsonValue(std::shared_ptr<JsonObject> v) : value_(v) {}
JsonValue::JsonValue(JsonObject v) : value_(std::make_shared<JsonObject>(v)) {}
JsonValue::JsonValue(std::shared_ptr<JsonArray> v) : value_(v) {}
JsonValue::JsonValue(JsonArray v) : value_(std::make_shared<JsonArray>(v)) {}

absl::StatusOr<JsonValue> JsonValue::Parse(std::string_view str) {
  auto full_str = str;
  ASSIGN_OR_RETURN(auto result, ParseValue(str));
  // str should be empty except for trailing whitespace.
  SkipLeadingWhitespace(str);
  if (str.empty()) {
    return result;
  }
  return absl::InvalidArgumentError(
      absl::StrCat("Expected only trailing whitespace at offset ",
                   str.data() - full_str.data()));
}

JsonValue::EType JsonValue::type() const {
  return static_cast<EType>(value_.index());
}

bool JsonValue::as_bool() const { return std::get<kBool>(value_); }

double JsonValue::as_number() const { return std::get<kNumber>(value_); }

const std::string& JsonValue::as_string() const {
  return std::get<kString>(value_);
}

const JsonObject& JsonValue::as_object() const {
  return *std::get<kObject>(value_);
}

const JsonArray& JsonValue::as_array() const {
  return *std::get<kArray>(value_);
}

bool JsonValue::HasKey(const std::string& key) const {
  return type() == kObject && as_object().HasKey(key);
}

JsonValue JsonValue::GetValue(const std::string& key) const {
  if (type() == kObject) {
    return as_object().GetValue(key);
  }
  return JsonValue();
}

bool JsonValue::HasIndex(size_t index) const {
  return type() == kArray && as_array().size() > index;
}

JsonValue JsonValue::GetElement(size_t index) const {
  if (type() == kArray && as_array().size() > index) {
    return as_array()[index];
  }
  return JsonValue();
}

bool operator==(const JsonValue& jv, nullptr_t) {
  return jv.type() == JsonValue::kNull;
}

bool operator==(const JsonValue& jv, bool b) {
  return jv.type() == JsonValue::kBool && jv.as_bool() == b;
}

bool operator==(const JsonValue& jv, double d) {
  if (jv.type() != JsonValue::kNumber) {
    return false;
  }
  double a = jv.as_number(), b = d;
  if (a == b) {
    return true;
  }
  if (std::signbit(a) != std::signbit(b)) {
    return false;
  }
  if (!std::isnormal(a) || !std::isnormal(b)) {
    return false;
  }
  if (std::nexttoward(a, b) == b) {
    return true;
  }
  if (std::nexttoward(b, a) == a) {
    return true;
  }
  return false;
}

bool operator==(const JsonValue& jv, std::string_view s) {
  return jv.type() == JsonValue::kString && jv.as_string() == s;
}

bool operator==(const JsonValue& jv, const JsonArray& v) {
  return jv.type() == JsonValue::kArray && jv.as_array() == v;
}

bool operator==(const JsonValue& jv, const JsonObject& v) {
  return jv.type() == JsonValue::kObject && jv.as_object() == v;
}

bool operator==(const JsonValue& a, const JsonValue& b) {
  if (a.type() != b.type()) {
    return false;
  }
  switch (a.type()) {
    case JsonValue::kUnset:
      return true;
    case JsonValue::kNull:
      return true;
    case JsonValue::kBool:
      return a.as_bool() == b.as_bool();
    case JsonValue::kNumber:
      return a == b.as_number();
    case JsonValue::kString:
      return a.as_string() == b.as_string();
    case JsonValue::kObject:
      return a.as_object() == b.as_object();
    case JsonValue::kArray:
      return a.as_array() == b.as_array();
  }
}

std::ostream& operator<<(std::ostream& os, const JsonValue& jv) {
  os << "{type: ";

  switch (jv.type()) {
    case JsonValue::kUnset:
      return os << "Unset}";

    case JsonValue::kNull:
      return os << "Null}";

    case JsonValue::kBool:
      os << "Bool, value: ";
      if (jv.as_bool()) {
        return os << "true}";
      } else {
        return os << "false}";
      }

    case JsonValue::kNumber:
      return os << "Number, value: " << jv.as_number() << "}";

    case JsonValue::kString:
      return os << "String, value: \"" << absl::CHexEscape(jv.as_string())
                << "\"}";

    case JsonValue::kObject:
      return os << "Object, value: " << ::testing::PrintToString(jv.as_object())
                << "}";

    case JsonValue::kArray:
      return os << "Array, value: " << ::testing::PrintToString(jv.as_array())
                << "}";
  }
}

bool JsonObject::HasKey(const std::string& key) const {
  return find(key) != end();
}

JsonValue JsonObject::GetValue(const std::string& key) const {
  auto iter = find(key);
  if (iter == end()) {
    return JsonValue();
  } else {
    return iter->second;
  }
}

}  // namespace test
}  // namespace alpaca

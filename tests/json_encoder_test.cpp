#include "json_encoder.h"

// Tests of JsonObjectEncoder and JsonArrayEncoder.
//
// Author: james.synge@gmail.com

#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <string>

#include "absl/strings/str_cat.h"
#include "counting_bitbucket.h"
#include "googletest/gtest.h"
#include "logging.h"
#include "tests/test_utils.h"

namespace alpaca {
namespace {

class JsonEncodersTest : public testing::Test {
 protected:
  void ConfirmEncoding(std::function<void(Print&)> encoder,
                       const std::string& expected) {
    // Confirm that we print the expected characters.
    {
      PrintToString out;
      encoder(out);
      ASSERT_EQ(out.str(), expected);
      DVLOG(1) << "Output: " << out.str();
    }
    // Confirm that we count the expected number of characters.
    {
      CountingBitbucket out;
      encoder(out);
      ASSERT_EQ(out.count(), expected.size());
    }
  }
};

TEST_F(JsonEncodersTest, EmptyObject) {
  auto fn = [](Print& out) {
    JsonObjectEncoder object_encoder(out);
    LOG(INFO) << "sizeof(JsonObjectEncoder): " << sizeof(JsonObjectEncoder);
  };
  ConfirmEncoding(fn, "{}");
}

TEST_F(JsonEncodersTest, ObjectWithStringValues) {
  auto fn = [](Print& out) {
    JsonObjectEncoder object_encoder(out);
    object_encoder.AddStringProperty("empty", "");
    object_encoder.AddStringProperty("a", "some text");
    object_encoder.AddStringProperty("b", "with \" quotes and \\ backslashes");
    object_encoder.AddStringProperty("c", "with controls \r\n");
  };
  ConfirmEncoding(fn,
                  "{\"empty\": \"\", "
                  "\"a\": \"some text\", "
                  "\"b\": \"with \\\" quotes and \\\\ backslashes\", "
                  "\"c\": \"with controls \\r\\n\"}");
}

TEST_F(JsonEncodersTest, ObjectWithBooleanValues) {
  auto fn = [](Print& out) {
    JsonObjectEncoder object_encoder(out);
    object_encoder.AddBooleanProperty("So true!", true);
    object_encoder.AddBooleanProperty("Heck no!", false);
  };
  ConfirmEncoding(fn, "{\"So true!\": true, \"Heck no!\": false}");
}

TEST_F(JsonEncodersTest, ObjectWithIntegerValues) {
  auto fn = [](Print& out) {
    JsonObjectEncoder object_encoder(out);
    object_encoder.AddIntegerProperty("min int8",
                                      std::numeric_limits<int8_t>::min());
    object_encoder.AddIntegerProperty("max int8",
                                      std::numeric_limits<int8_t>::max());
    object_encoder.AddIntegerProperty("min uint32",
                                      std::numeric_limits<uint32_t>::min());
    object_encoder.AddIntegerProperty("max uint32",
                                      std::numeric_limits<uint32_t>::max());
  };
  ConfirmEncoding(fn,
                  "{\"min int8\": -128, \"max int8\": 127, "
                  "\"min uint32\": 0, \"max uint32\": 4294967295}");
}

TEST_F(JsonEncodersTest, ObjectWithFloatValues) {
  auto fn = [](Print& out) {
    JsonObjectEncoder object_encoder(out);
    object_encoder.AddFloatingPointProperty("float zero",
                                            static_cast<float>(0));
    object_encoder.AddFloatingPointProperty("float one", static_cast<float>(1));

    EXPECT_TRUE(std::numeric_limits<float>::has_infinity);
    object_encoder.AddFloatingPointProperty(
        "float -Inf", -std::numeric_limits<float>::infinity());
    object_encoder.AddFloatingPointProperty(
        "float Inf", std::numeric_limits<float>::infinity());

    object_encoder.AddFloatingPointProperty("float NaN", std::nanf("99999999"));
  };
  ConfirmEncoding(
      fn,
      absl::StrCat("{\"float zero\": ", std::to_string(static_cast<float>(0)),
                   ", \"float one\": ", std::to_string(static_cast<float>(1)),
                   ", \"float -Inf\": \"-Inf\", \"float Inf\": \"Inf\""
                   ", \"float NaN\": \"NaN\"}"));
}

TEST_F(JsonEncodersTest, ObjectWithDoubleValues) {
  auto fn = [](Print& out) {
    JsonObjectEncoder object_encoder(out);
    object_encoder.AddFloatingPointProperty("double zero",
                                            static_cast<double>(0));
    object_encoder.AddFloatingPointProperty("double one",
                                            static_cast<double>(1));

    EXPECT_TRUE(std::numeric_limits<double>::has_infinity);
    object_encoder.AddFloatingPointProperty(
        "double -Inf", -std::numeric_limits<double>::infinity());
    object_encoder.AddFloatingPointProperty(
        "double Inf", std::numeric_limits<double>::infinity());

    object_encoder.AddFloatingPointProperty("double NaN", std::nan("1"));
  };
  ConfirmEncoding(
      fn,
      absl::StrCat("{\"double zero\": ", std::to_string(static_cast<double>(0)),
                   ", \"double one\": ", std::to_string(static_cast<double>(1)),
                   ", \"double -Inf\": \"-Inf\", \"double Inf\": \"Inf\""
                   ", \"double NaN\": \"NaN\"}"));
}

TEST_F(JsonEncodersTest, ObjectWithArrayValues) {
  const double kPi = 3.14159265359L;

  auto fn = [kPi](Print& out) {
    JsonObjectEncoder object_encoder(out);
    { auto array_encoder = object_encoder.StartArrayProperty("empty"); }
    {
      auto array_encoder = object_encoder.StartArrayProperty("mixed");
      array_encoder.AddBooleanElement(true);
      array_encoder.AddFloatingPointElement(kPi);
      array_encoder.AddIntegerElement(43);
      array_encoder.AddStringElement("xyzzy");
    }
  };
  ConfirmEncoding(fn, absl::StrCat("{\"empty\": [], \"mixed\": [true, ",
                                   std::to_string(kPi), ", 43, \"xyzzy\"]}"));
}

TEST_F(JsonEncodersTest, ObjectWithObjectValues) {
  const double kPi = 3.14159265359L;

  auto fn = [kPi](Print& out) {
    JsonObjectEncoder object_encoder(out);
    object_encoder.StartObjectProperty("empty");
    {
      auto object_encoder2 = object_encoder.StartObjectProperty("mixed");
      object_encoder2.AddBooleanProperty("Too darn true!", true);
      object_encoder2.AddFloatingPointProperty("Gimme some pie!", kPi);
    }
  };
  ConfirmEncoding(
      fn, absl::StrCat("{\"empty\": {}, \"mixed\": {\"Too darn true!\": true, ",
                       "\"Gimme some pie!\": ", std::to_string(kPi), "}}"));
}

TEST_F(JsonEncodersTest, EmptyArray) {
  auto fn = [](Print& out) {
    JsonArrayEncoder array_encoder(out);
    LOG(INFO) << "sizeof(JsonArrayEncoder): " << sizeof(JsonArrayEncoder);
  };
  ConfirmEncoding(fn, "[]");
}

TEST_F(JsonEncodersTest, ArrayOfEmptyStructures) {
  auto fn = [](Print& out) {
    JsonArrayEncoder array_encoder(out);
    array_encoder.StartArrayElement();
    array_encoder.StartObjectElement();
  };
  ConfirmEncoding(fn, "[[], {}]");
}

TEST_F(JsonEncodersTest, ArrayOfMixedValueTypes) {
  auto fn = [](Print& out) {
    JsonArrayEncoder array_encoder(out);
    array_encoder.AddBooleanElement(false);
    array_encoder.AddStringElement("");
    array_encoder.AddStringElement("some text \r\n with escaping characters");
    array_encoder.AddIntegerElement(std::numeric_limits<uint8_t>::max());
    array_encoder.AddFloatingPointElement(-1.0F);
    {
      auto object_encoder = array_encoder.StartObjectElement();
      object_encoder.StartArrayProperty("inner-empty-array");
    }
  };
  ConfirmEncoding(
      fn, absl::StrCat("[false, \"\", "
                       "\"some text \\r\\n with escaping characters\", ",
                       std::to_string(std::numeric_limits<uint8_t>::max() + 0),
                       ", ", std::to_string(-1.0F),
                       ", {\"inner-empty-array\": []}]"));
}

}  // namespace
}  // namespace alpaca

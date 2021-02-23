#include "src/json_encoder.h"

// Tests of JsonObjectEncoder and JsonArrayEncoder.
//
// Author: james.synge@gmail.com

#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <string>

#include "absl/strings/str_cat.h"
#include "extras/tests/test_utils.h"
#include "googletest/gtest.h"
#include "logging.h"
#include "src/counting_bitbucket.h"

namespace alpaca {
namespace {

class JsonEncodersTest : public testing::Test {
 protected:
  void ConfirmEncoding(const JsonElementSourceFunction& func,
                       const std::string& expected) {
    // Confirm that we print the expected characters.
    {
      PrintToString out;
      JsonArrayEncoder::Encode(func, out);
      ASSERT_EQ(out.str(), expected);
      DVLOG(1) << "Output: " << out.str();
    }
    // Confirm that we count the expected number of characters.
    {
      CountingBitbucket out;
      JsonArrayEncoder::Encode(func, out);
      ASSERT_EQ(out.count(), expected.size());
    }
  }
  void ConfirmEncoding(const JsonPropertySourceFunction& func,
                       const std::string& expected) {
    // Confirm that we print the expected characters.
    {
      PrintToString out;
      JsonObjectEncoder::Encode(func, out);
      ASSERT_EQ(out.str(), expected);
      DVLOG(1) << "Output: " << out.str();
    }
    // Confirm that we count the expected number of characters.
    {
      CountingBitbucket out;
      JsonObjectEncoder::Encode(func, out);
      ASSERT_EQ(out.count(), expected.size());
    }
  }
};

TEST_F(JsonEncodersTest, InstanceSizes) {
  LOG(INFO) << "sizeof(JsonArrayEncoder): " << sizeof(JsonArrayEncoder);
  LOG(INFO) << "sizeof(JsonObjectEncoder): " << sizeof(JsonObjectEncoder);
  LOG(INFO) << "sizeof(JsonElementSource): " << sizeof(JsonElementSource);
  LOG(INFO) << "sizeof(JsonPropertySource): " << sizeof(JsonPropertySource);
}

TEST_F(JsonEncodersTest, EmptyObject) {
  auto func = [](JsonObjectEncoder& encoder) {};
  ConfirmEncoding(func, "{}");
}

TEST_F(JsonEncodersTest, ObjectWithStringValues) {
  auto func = [](JsonObjectEncoder& object_encoder) {
    object_encoder.AddStringProperty("empty", "");
    object_encoder.AddStringProperty("a", "some text");
    object_encoder.AddStringProperty("b", "with \" quotes and \\ backslashes");
    object_encoder.AddStringProperty("c", "with controls \r\n");
  };
  ConfirmEncoding(func,
                  "{\"empty\": \"\", "
                  "\"a\": \"some text\", "
                  "\"b\": \"with \\\" quotes and \\\\ backslashes\", "
                  "\"c\": \"with controls \\r\\n\"}");
}

TEST_F(JsonEncodersTest, ObjectWithBooleanValues) {
  auto func = [](JsonObjectEncoder& object_encoder) {
    object_encoder.AddBooleanProperty("So true!", true);
    object_encoder.AddBooleanProperty("Heck no!", false);
  };
  ConfirmEncoding(func, "{\"So true!\": true, \"Heck no!\": false}");
}

TEST_F(JsonEncodersTest, ObjectWithIntegerValues) {
  auto func = [](JsonObjectEncoder& object_encoder) {
    object_encoder.AddIntegerProperty("min int8",
                                      std::numeric_limits<int8_t>::min());
    object_encoder.AddIntegerProperty("max int8",
                                      std::numeric_limits<int8_t>::max());

    object_encoder.AddIntegerProperty("min int32",
                                      std::numeric_limits<int32_t>::min());
    object_encoder.AddIntegerProperty("max int32",
                                      std::numeric_limits<int32_t>::max());

    object_encoder.AddIntegerProperty("min uint32",
                                      std::numeric_limits<uint32_t>::min());
    object_encoder.AddIntegerProperty("max uint32",
                                      std::numeric_limits<uint32_t>::max());
  };
  ConfirmEncoding(func,
                  "{\"min int8\": -128, \"max int8\": 127, "
                  "\"min int32\": -2147483648, \"max int32\": 2147483647, "
                  "\"min uint32\": 0, \"max uint32\": 4294967295}");
}

TEST_F(JsonEncodersTest, ObjectWithFloatValues) {
  auto func = [](JsonObjectEncoder& object_encoder) {
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
      func,
      absl::StrCat("{\"float zero\": ", std::to_string(static_cast<float>(0)),
                   ", \"float one\": ", std::to_string(static_cast<float>(1)),
                   ", \"float -Inf\": \"-Inf\", \"float Inf\": \"Inf\""
                   ", \"float NaN\": \"NaN\"}"));
}

TEST_F(JsonEncodersTest, ObjectWithDoubleValues) {
  auto func = [](JsonObjectEncoder& object_encoder) {
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
      func,
      absl::StrCat("{\"double zero\": ", std::to_string(static_cast<double>(0)),
                   ", \"double one\": ", std::to_string(static_cast<double>(1)),
                   ", \"double -Inf\": \"-Inf\", \"double Inf\": \"Inf\""
                   ", \"double NaN\": \"NaN\"}"));
}

TEST_F(JsonEncodersTest, ObjectWithArrayValues) {
  const double kPi = 3.14159265359L;
  auto func = [kPi](JsonObjectEncoder& object_encoder) {
    object_encoder.AddArrayProperty("empty",
                                    [](JsonArrayEncoder& array_encoder) {});
    object_encoder.AddArrayProperty(
        "mixed", [kPi](JsonArrayEncoder& array_encoder) {
          array_encoder.AddBooleanElement(true);
          array_encoder.AddFloatingPointElement(kPi);
          array_encoder.AddIntegerElement(43);
          array_encoder.AddStringElement("xyzzy");
        });
  };
  ConfirmEncoding(func, absl::StrCat("{\"empty\": [], \"mixed\": [true, ",
                                     std::to_string(kPi), ", 43, \"xyzzy\"]}"));
}

TEST_F(JsonEncodersTest, ObjectWithObjectValues) {
  const double kPi = 3.14159265359L;
  auto func = [kPi](JsonObjectEncoder& object_encoder) {
    object_encoder.AddObjectProperty("empty", [](JsonObjectEncoder&) {});
    object_encoder.AddObjectProperty(
        "mixed", [kPi](JsonObjectEncoder& object_encoder) {
          object_encoder.AddBooleanProperty("Too darn true!", true);
          object_encoder.AddFloatingPointProperty("Gimme some pie!", kPi);
        });
  };
  ConfirmEncoding(
      func,
      absl::StrCat("{\"empty\": {}, \"mixed\": {\"Too darn true!\": true, ",
                   "\"Gimme some pie!\": ", std::to_string(kPi), "}}"));
}

TEST_F(JsonEncodersTest, EmptyArray) {
  ConfirmEncoding([](JsonArrayEncoder& array_encoder) {}, "[]");
}

TEST_F(JsonEncodersTest, ArrayOfEmptyStructures) {
  auto func = [](JsonArrayEncoder& array_encoder) {
    array_encoder.AddArrayElement([](JsonArrayEncoder&) {});
    array_encoder.AddObjectElement([](JsonObjectEncoder&) {});
  };
  ConfirmEncoding(func, "[[], {}]");
}

TEST_F(JsonEncodersTest, ArrayOfMixedValueTypes) {
  auto func = [](JsonArrayEncoder& array_encoder) {
    array_encoder.AddBooleanElement(false);
    array_encoder.AddStringElement("");
    array_encoder.AddStringElement("some text \r\n with escaping characters");
    array_encoder.AddIntegerElement(std::numeric_limits<int32_t>::min());
    array_encoder.AddIntegerElement(std::numeric_limits<uint32_t>::max());
    array_encoder.AddFloatingPointElement(-1.0F);
    array_encoder.AddObjectElement([](JsonObjectEncoder& object_encoder) {
      object_encoder.AddArrayProperty("inner-empty-array",
                                      [](JsonArrayEncoder&) {});
    });
  };
  ConfirmEncoding(
      func, absl::StrCat(
                "[false, \"\", "
                "\"some text \\r\\n with escaping characters\", ",
                std::to_string(std::numeric_limits<int32_t>::min() + 0), ", ",
                std::to_string(std::numeric_limits<uint32_t>::max() + 0), ", ",
                std::to_string(-1.0F), ", {\"inner-empty-array\": []}]"));
}

}  // namespace
}  // namespace alpaca

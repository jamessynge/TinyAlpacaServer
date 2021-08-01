#include "utils/json_encoder.h"

#include <stdint.h>

#include <cmath>
#include <limits>
#include <string>

#include "absl/strings/str_cat.h"
#include "extras/test_tools/json_test_utils.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"
#include "mcucore/extrastest_tools/sample_printable.h"
#include "utils/literal.h"
#include "utils/string_view.h"

// Tests of JsonObjectEncoder and JsonArrayEncoder.
//
// Author: james.synge@gmail.com

namespace alpaca {
namespace test {
namespace {

class JsonEncodersTest : public testing::Test {
 protected:
  void ConfirmEncoding(const JsonElementSourceFunction& func,
                       const std::string& expected) {
    ElementSourceFunctionAdapter source(func);
    // Confirm that we print the expected characters.
    {
      mcucore::test::PrintToStdString out;
      JsonArrayEncoder::Encode(source, out);
      ASSERT_EQ(out.str(), expected);
      DVLOG(1) << "Output: " << out.str();
    }
    // Confirm that we count the expected number of characters.
    ASSERT_EQ(JsonArrayEncoder::EncodedSize(source), expected.size());
  }

  void ConfirmEncoding(const JsonPropertySourceFunction& func,
                       const std::string& expected) {
    PropertySourceFunctionAdapter source(func);
    // Confirm that we print the expected characters.
    {
      mcucore::test::PrintToStdString out;
      JsonObjectEncoder::Encode(source, out);
      ASSERT_EQ(out.str(), expected);
      DVLOG(1) << "Output: " << out.str();
    }
    // Confirm that we count the expected number of characters.
    ASSERT_EQ(JsonObjectEncoder::EncodedSize(source), expected.size());
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
  constexpr char kSomeTextStr[] = "some text";
  const Literal kSomeText(kSomeTextStr);
  constexpr char kWithQuotesAndBackslashesStr[] =
      "with \" quotes and \\ backslashes";
  const Literal kWithQuotesAndBackslashes(kWithQuotesAndBackslashesStr);
  const mcucore::test::SamplePrintable kPrintableValue("with controls \r\n");

  auto func = [&](JsonObjectEncoder& object_encoder) {
    object_encoder.AddStringProperty(StringView("empty"), StringView());
    object_encoder.AddStringProperty(Literal("a"), kSomeText);
    object_encoder.AddStringProperty(StringView("b"),
                                     kWithQuotesAndBackslashes);
    object_encoder.AddStringProperty(StringView("c"), kPrintableValue);
  };
  ConfirmEncoding(func,
                  "{\"empty\": \"\", "
                  "\"a\": \"some text\", "
                  "\"b\": \"with \\\" quotes and \\\\ backslashes\", "
                  "\"c\": \"with controls \\r\\n\"}");
}

TEST_F(JsonEncodersTest, ObjectWithBooleanValues) {
  auto func = [](JsonObjectEncoder& object_encoder) {
    object_encoder.AddBooleanProperty(StringView("So true!"), true);
    object_encoder.AddBooleanProperty(StringView("Heck no!"), false);
  };
  ConfirmEncoding(func, "{\"So true!\": true, \"Heck no!\": false}");
}

TEST_F(JsonEncodersTest, ObjectWithIntegerValues) {
  auto func = [](JsonObjectEncoder& object_encoder) {
    object_encoder.AddIntProperty(StringView("min int8"),
                                  std::numeric_limits<int8_t>::min());
    object_encoder.AddIntProperty(StringView("max int8"),
                                  std::numeric_limits<int8_t>::max());

    object_encoder.AddIntProperty(StringView("min int32"),
                                  std::numeric_limits<int32_t>::min());
    object_encoder.AddIntProperty(StringView("max int32"),
                                  std::numeric_limits<int32_t>::max());

    object_encoder.AddUIntProperty(StringView("min uint32"),
                                   std::numeric_limits<uint32_t>::min());
    object_encoder.AddUIntProperty(StringView("max uint32"),
                                   std::numeric_limits<uint32_t>::max());
  };
  ConfirmEncoding(func,
                  "{\"min int8\": -128, \"max int8\": 127, "
                  "\"min int32\": -2147483648, \"max int32\": 2147483647, "
                  "\"min uint32\": 0, \"max uint32\": 4294967295}");
}

TEST_F(JsonEncodersTest, ObjectWithFloatValues) {
  auto func = [](JsonObjectEncoder& object_encoder) {
    object_encoder.AddFloatProperty(StringView("float zero"),
                                    static_cast<float>(0));
    object_encoder.AddFloatProperty(StringView("float one"),
                                    static_cast<float>(1));

    EXPECT_TRUE(std::numeric_limits<float>::has_infinity);
    object_encoder.AddFloatProperty(StringView("float -Inf"),
                                    -std::numeric_limits<float>::infinity());
    object_encoder.AddFloatProperty(StringView("float Inf"),
                                    std::numeric_limits<float>::infinity());

    object_encoder.AddFloatProperty(StringView("float NaN"),
                                    std::nanf("99999999"));
  };
  ConfirmEncoding(
      func,
      absl::StrCat("{\"float zero\": ", "0.00", ", \"float one\": ", "1.00",
                   ", \"float -Inf\": \"-Inf\", \"float Inf\": \"Inf\""
                   ", \"float NaN\": \"NaN\"}"));
}

TEST_F(JsonEncodersTest, ObjectWithDoubleValues) {
  auto func = [](JsonObjectEncoder& object_encoder) {
    object_encoder.AddDoubleProperty(StringView("double zero"),
                                     static_cast<double>(0));
    object_encoder.AddDoubleProperty(StringView("double one"),
                                     static_cast<double>(1));

    EXPECT_TRUE(std::numeric_limits<double>::has_infinity);
    object_encoder.AddDoubleProperty(StringView("double -Inf"),
                                     -std::numeric_limits<double>::infinity());
    object_encoder.AddDoubleProperty(StringView("double Inf"),
                                     std::numeric_limits<double>::infinity());

    object_encoder.AddDoubleProperty(StringView("double NaN"), std::nan("1"));
  };
  ConfirmEncoding(
      func,
      absl::StrCat("{\"double zero\": ", "0.00", ", \"double one\": ", "1.00",
                   ", \"double -Inf\": \"-Inf\", \"double Inf\": \"Inf\""
                   ", \"double NaN\": \"NaN\"}"));
}

TEST_F(JsonEncodersTest, ObjectWithArrayValues) {
  const double kPi = 3.14159265359L;
  auto func = [kPi](JsonObjectEncoder& object_encoder) {
    AddArrayProperty(object_encoder, StringView("empty"),
                     [](JsonArrayEncoder& array_encoder) {});
    AddArrayProperty(object_encoder, StringView("mixed"),
                     [kPi](JsonArrayEncoder& array_encoder) {
                       array_encoder.AddBooleanElement(true);
                       array_encoder.AddDoubleElement(kPi);
                       array_encoder.AddIntElement(43);
                       array_encoder.AddStringElement(StringView("xyzzy"));
                     });
  };
  ConfirmEncoding(func, absl::StrCat("{\"empty\": [], \"mixed\": [true, ",
                                     "3.14", ", 43, \"xyzzy\"]}"));
}

TEST_F(JsonEncodersTest, ObjectWithObjectValues) {
  const double kPi = 3.14159265359L;
  auto func = [kPi](JsonObjectEncoder& object_encoder) {
    AddObjectProperty(object_encoder, StringView("empty"),
                      [](JsonObjectEncoder&) {});
    AddObjectProperty(
        object_encoder, StringView("mixed"),
        [kPi](JsonObjectEncoder& object_encoder) {
          object_encoder.AddBooleanProperty(StringView("Too darn true!"), true);
          object_encoder.AddDoubleProperty(StringView("Gimme some pie!"), kPi);
        });
  };
  ConfirmEncoding(
      func,
      absl::StrCat("{\"empty\": {}, \"mixed\": {\"Too darn true!\": true, ",
                   "\"Gimme some pie!\": ", "3.14", "}}"));
}

TEST_F(JsonEncodersTest, EmptyArray) {
  ConfirmEncoding([](JsonArrayEncoder& array_encoder) {}, "[]");
}

TEST_F(JsonEncodersTest, ArrayOfEmptyStructures) {
  auto func = [](JsonArrayEncoder& array_encoder) {
    AddArrayElement(array_encoder, [](JsonArrayEncoder&) {});
    AddObjectElement(array_encoder, [](JsonObjectEncoder&) {});
  };
  ConfirmEncoding(func, "[[], {}]");
}

TEST_F(JsonEncodersTest, ArrayOfMixedValueTypes) {
  constexpr char kSomeTextStr[] = "some text \r\n with escaping characters";
  const Literal kSomeText(kSomeTextStr);
  const mcucore::test::SamplePrintable kPrintableValue("just printable");

  auto func = [&](JsonArrayEncoder& array_encoder) {
    array_encoder.AddBooleanElement(false);
    array_encoder.AddStringElement(StringView());  // Empty string.
    array_encoder.AddStringElement(kPrintableValue);
    array_encoder.AddStringElement(kSomeText);
    array_encoder.AddIntElement(std::numeric_limits<int32_t>::min());
    array_encoder.AddUIntElement(std::numeric_limits<uint32_t>::max());
    array_encoder.AddFloatElement(-1.0F);
    AddObjectElement(array_encoder, [](JsonObjectEncoder& object_encoder) {
      AddArrayProperty(object_encoder, StringView("inner-empty-array"),
                       [](JsonArrayEncoder&) {});
    });
  };
  ConfirmEncoding(
      func,
      absl::StrCat("[false, \"\", \"just printable\", "
                   "\"some text \\r\\n with escaping characters\", ",
                   std::to_string(std::numeric_limits<int32_t>::min()), ", ",
                   std::to_string(std::numeric_limits<uint32_t>::max()), ", ",
                   "-1.00", ", {\"inner-empty-array\": []}]"));
}

}  // namespace
}  // namespace test
}  // namespace alpaca

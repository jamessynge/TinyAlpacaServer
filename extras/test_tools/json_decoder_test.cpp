#include "extras/test_tools/json_decoder.h"

#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace test {
namespace {

using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::SizeIs;
using ::testing::UnorderedElementsAre;

#define WHITESPACE " \n\r\t"

TEST(JsonDecoderTest, Unset) {
  JsonValue value;
  EXPECT_EQ(value.type(), JsonValue::kUnset);
  EXPECT_TRUE(value.is_unset());
  EXPECT_EQ(value, JsonValue());
  EXPECT_NE(value, JsonValue(nullptr));
}

TEST(JsonDecoderTest, Null) {
  ASSERT_OK_AND_ASSIGN(auto value,
                       JsonValue::Parse(WHITESPACE "null" WHITESPACE));
  ASSERT_EQ(value.type(), JsonValue::kNull);

  ASSERT_EQ(value, JsonValue(nullptr));
  ASSERT_EQ(JsonValue(nullptr), value);

  EXPECT_NE(value, JsonValue(false));
  EXPECT_NE(JsonValue(""), value);
}

TEST(JsonDecoderTest, True) {
  ASSERT_OK_AND_ASSIGN(auto value,
                       JsonValue::Parse(WHITESPACE "true" WHITESPACE));
  ASSERT_EQ(value.type(), JsonValue::kBool);

  EXPECT_EQ(value.as_bool(), true);
  EXPECT_EQ(true, value.as_bool());
  EXPECT_EQ(value, JsonValue(true));
  EXPECT_EQ(JsonValue(true), value);

  EXPECT_NE(value, JsonValue(nullptr));
  EXPECT_NE(value, false);
}

TEST(JsonDecoderTest, False) {
  ASSERT_OK_AND_ASSIGN(auto value,
                       JsonValue::Parse(WHITESPACE "false" WHITESPACE));
  ASSERT_EQ(value.type(), JsonValue::kBool);

  EXPECT_EQ(value.as_bool(), false);
  EXPECT_EQ(false, value.as_bool());
  EXPECT_EQ(value, JsonValue(false));
  EXPECT_EQ(JsonValue(false), value);

  EXPECT_NE(value, JsonValue("false"));
  EXPECT_NE(value, true);
}

TEST(JsonDecoderTest, Zero) {
  ASSERT_OK_AND_ASSIGN(auto value,
                       JsonValue::Parse(WHITESPACE "-0.0E+0" WHITESPACE));
  ASSERT_EQ(value.type(), JsonValue::kNumber);

  EXPECT_EQ(value.as_number(), -0.0);
  EXPECT_EQ(value, JsonValue(-0.0));
  EXPECT_EQ(JsonValue(-0.0), value);

  // Defined -0 == +0.
  EXPECT_EQ(value, JsonValue(0.0));
  EXPECT_EQ(JsonValue(0.0), value);

  EXPECT_NE(value, JsonValue(nullptr));
  EXPECT_NE(value, 0.0000000001);
  EXPECT_NE(value, -0.0000000001);
}

TEST(JsonDecoderTest, One) {
  ASSERT_OK_AND_ASSIGN(auto value,
                       JsonValue::Parse(WHITESPACE "1.0e-0" WHITESPACE));
  ASSERT_EQ(value.type(), JsonValue::kNumber);

  EXPECT_EQ(1.0, value.as_number());
  EXPECT_EQ(value.as_number(), 1.0);
  EXPECT_EQ(value, JsonValue(1.0));
  EXPECT_EQ(JsonValue(1.0), value);
}

TEST(JsonDecoderTest, EmptyString) {
  ASSERT_OK_AND_ASSIGN(auto value,
                       JsonValue::Parse(WHITESPACE "\"\"" WHITESPACE));
  ASSERT_EQ(value.type(), JsonValue::kString);

  EXPECT_EQ(value.as_string(), "");
  EXPECT_EQ("", value.as_string());
  EXPECT_EQ(value, JsonValue(""));
  EXPECT_EQ(JsonValue(""), value);
}

TEST(JsonDecoderTest, AllSupportedAsciiCharsString) {
  ASSERT_OK_AND_ASSIGN(
      auto value,
      JsonValue::Parse(WHITESPACE
                       R"("\"\\\/\b\f\n\r\t !#$%&'()*+,-./0123456789:;<=>?)@)"
                       R"(ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`)"
                       R"(abcdefghijklmnopqrstuvwxyz{|}~")" WHITESPACE));
  ASSERT_EQ(value.type(), JsonValue::kString);
  EXPECT_EQ(value.as_string(),
            "\"\\/\b\f\n\r\t !#$%&'()*+,-./0123456789:;<=>?)@"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`"
            "abcdefghijklmnopqrstuvwxyz{|}~");
}

TEST(JsonDecoderTest, EmptyObject) {
  ASSERT_OK_AND_ASSIGN(
      auto value, JsonValue::Parse(WHITESPACE "{" WHITESPACE "}" WHITESPACE));
  ASSERT_EQ(value.type(), JsonValue::kObject);
  EXPECT_THAT(value.as_object(), IsEmpty());

  ASSERT_OK_AND_ASSIGN(value, JsonValue::Parse("{}"));
  ASSERT_EQ(value.type(), JsonValue::kObject);
  EXPECT_THAT(value.as_object(), IsEmpty());
}

TEST(JsonDecoderTest, ObjectWithOneEntry) {
  ASSERT_OK_AND_ASSIGN(auto value, JsonValue::Parse(R"({ "key" : " " })"));
  ASSERT_EQ(value.type(), JsonValue::kObject);
  EXPECT_THAT(value.as_object(), SizeIs(1));
  EXPECT_THAT(value.as_object(),
              UnorderedElementsAre(Pair("key", JsonValue(" "))));
}

TEST(JsonDecoderTest, ObjectWithMultipleEntries) {
  ASSERT_OK_AND_ASSIGN(
      auto value,
      JsonValue::Parse(
          R"({"a":null,"b":true,"c":false,"d":-0,"e":"","f":{},"g":[]})"));
  ASSERT_EQ(value.type(), JsonValue::kObject);

  JsonValue expected(JsonObject()
                         .Add("a", nullptr)
                         .Add("b", true)
                         .Add("c", false)
                         .Add("d", -0.0)
                         .Add("e", "")
                         .Add("f", JsonObject())
                         .Add("g", JsonArray()));

  EXPECT_EQ(value, expected);
}

TEST(JsonDecoderTest, EmptyArray) {
  ASSERT_OK_AND_ASSIGN(
      auto value, JsonValue::Parse(WHITESPACE "[" WHITESPACE "]" WHITESPACE));
  ASSERT_EQ(value.type(), JsonValue::kArray);
  EXPECT_THAT(value.as_array(), IsEmpty());

  ASSERT_OK_AND_ASSIGN(value, JsonValue::Parse("[]"));
  ASSERT_EQ(value.type(), JsonValue::kArray);
  EXPECT_THAT(value.as_array(), IsEmpty());
}

TEST(JsonDecoderTest, ArrayWithOneNumberEntry) {
  ASSERT_OK_AND_ASSIGN(auto value, JsonValue::Parse(R"([0.1])"));
  ASSERT_EQ(value.type(), JsonValue::kArray);
  EXPECT_THAT(value.as_array(), SizeIs(1));
  EXPECT_THAT(value.as_array(), ElementsAre(JsonValue(0.1)));
}

TEST(JsonDecoderTest, ArrayWithMultipleEntries) {
  ASSERT_OK_AND_ASSIGN(auto value,
                       JsonValue::Parse(R"([[""],false,null,"",true,100,{}])"));
  ASSERT_EQ(value.type(), JsonValue::kArray);

  JsonValue expected(JsonArray()
                         .Add(JsonArray().Add(""))
                         .Add(false)
                         .Add(nullptr)
                         .Add("")
                         .Add(true)
                         .Add(100)
                         .Add(JsonObject()));

  EXPECT_EQ(value, expected);
}

}  // namespace
}  // namespace test
}  // namespace alpaca

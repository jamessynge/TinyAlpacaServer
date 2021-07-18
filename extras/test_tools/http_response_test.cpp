#include "extras/test_tools/http_response.h"

#include <optional>

#include "absl/status/status.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace test {
namespace {

using ::testing::IsEmpty;
using ::testing::Optional;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

TEST(HttpResponseTest, MinimalInput) {
  ASSERT_OK_AND_ASSIGN(auto hr, HttpResponse::Make("V 0\r\n\r\n"));
  EXPECT_EQ(hr.http_version, "V");
  EXPECT_EQ(hr.status_code, 0);
  EXPECT_EQ(hr.status_message, "");
  EXPECT_THAT(hr.headers, IsEmpty());
  EXPECT_EQ(hr.body_and_beyond, "");
}

TEST(HttpResponseTest, HasStatusMessageAndBody) {
  ASSERT_OK_AND_ASSIGN(
      auto hr,
      HttpResponse::Make("HTTP/1.1 200 OK, really\r\n\r\n\r\n\r\nFoo\r\n\r\n"));
  EXPECT_EQ(hr.http_version, "HTTP/1.1");
  EXPECT_EQ(hr.status_code, 200);
  EXPECT_EQ(hr.status_message, "OK, really");
  EXPECT_THAT(hr.headers, IsEmpty());
  EXPECT_EQ(hr.body_and_beyond, "\r\n\r\nFoo\r\n\r\n");
}

TEST(HttpResponseTest, Full) {
  ASSERT_OK_AND_ASSIGN(auto hr, HttpResponse::Make("HTTP/1.1 200 OK\r\n"
                                                   "Host: example.com\r\n"
                                                   "Content-Type:text/plain\r\n"
                                                   "Set-Cookie:  a = b\r\n"
                                                   "Set-Cookie:\t  c=d \t\r\n"
                                                   "\r\n"
                                                   "Some body."));
  EXPECT_EQ(hr.http_version, "HTTP/1.1");
  EXPECT_EQ(hr.status_code, 200);
  EXPECT_EQ(hr.status_message, "OK");
  EXPECT_THAT(hr.headers,
              UnorderedElementsAre(Pair("Host", "example.com"),
                                   Pair("Content-Type", "text/plain"),
                                   Pair("Set-Cookie", "a = b"),
                                   Pair("Set-Cookie", "c=d")));
  EXPECT_EQ(hr.body_and_beyond, "Some body.");
  EXPECT_EQ(hr.json_value, JsonValue());  // Unset
}

TEST(HttpResponseTest, EmptyJsonObject) {
  ASSERT_OK_AND_ASSIGN(auto hr,
                       HttpResponse::Make("HTTP/1.1 200 OK\r\n"
                                          "Content-Type: application/json \r\n"
                                          "Content-Length: 2\r\n"
                                          "\r\n"
                                          "{}  "));
  EXPECT_EQ(hr.http_version, "HTTP/1.1");
  EXPECT_EQ(hr.status_code, 200);
  EXPECT_EQ(hr.status_message, "OK");
  EXPECT_THAT(hr.headers,
              UnorderedElementsAre(Pair("Content-Type", "application/json"),
                                   Pair("Content-Length", "2")));
  EXPECT_EQ(hr.body_and_beyond, "  ");
  EXPECT_EQ(hr.json_value, JsonObject());
}

TEST(HttpResponseTest, TruncatedJsonResponse) {
  auto status_or = HttpResponse::Make(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: application/json \r\n"
      "Content-Length: 2\r\n"
      "\r\n"
      "{");

  ASSERT_FALSE(status_or.ok());
  ASSERT_TRUE(absl::IsInvalidArgument(status_or.status()))
      << status_or.status();
}

TEST(HttpResponseTest, CorruptJsonResponse) {
  auto status_or = HttpResponse::Make(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: application/json \r\n"
      "Content-Length: 2\r\n"
      "\r\n"
      "{]");

  ASSERT_FALSE(status_or.ok());
  ASSERT_TRUE(absl::IsInvalidArgument(status_or.status()))
      << status_or.status();
}

}  // namespace
}  // namespace test
}  // namespace alpaca

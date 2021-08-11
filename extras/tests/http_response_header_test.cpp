#include "http_response_header.h"

#include "absl/strings/str_cat.h"
#include "constants.h"
#include "gtest/gtest.h"
#include "literal.h"
#include "literals.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"

namespace alpaca {
namespace test {
namespace {

constexpr char kEOL[] = "\r\n";

TEST(HttpResponseHeaderTest, Json) {
  HttpResponseHeader hrh;
  hrh.status_code = EHttpStatusCode::kHttpOk;
  hrh.reason_phrase = Literals::OK();
  hrh.content_type = EContentType::kApplicationJson;
  hrh.content_length = 65535;

  mcucore::test::PrintToStdString out;
  hrh.printTo(out);
  EXPECT_EQ(
      out.str(),
      absl::StrCat("HTTP/1.1 200 OK", kEOL, "Server: TinyAlpacaServer", kEOL,
                   "Connection: close", kEOL, "Content-Type: application/json",
                   kEOL, "Content-Length: 65535", kEOL, kEOL));
}

TEST(HttpResponseHeaderTest, Error) {
  HttpResponseHeader hrh;
  hrh.status_code = EHttpStatusCode::kHttpBadRequest;
  hrh.reason_phrase = mcucore::Literal("Bad Request");
  hrh.content_type = EContentType::kTextPlain;
  hrh.content_length = 123;

  mcucore::test::PrintToStdString out;
  hrh.printTo(out);
  EXPECT_EQ(
      out.str(),
      absl::StrCat("HTTP/1.1 400 Bad Request", kEOL, "Server: TinyAlpacaServer",
                   kEOL, "Connection: close", kEOL, "Content-Type: text/plain",
                   kEOL, "Content-Length: 123", kEOL, kEOL));
}

}  // namespace
}  // namespace test
}  // namespace alpaca

#include "extras/test_tools/http_response.h"

#include <string.h>

#include <charconv>
#include <cstdlib>
#include <string>
#include <string_view>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "util/task/status_macros.h"

namespace alpaca {
namespace test {
namespace {

std::string TrimWhitespace(std::string str) {
  std::string_view linear_whitespace = " \t";  // From HTTP RFC.
  const auto start = str.find_first_not_of(linear_whitespace);
  if (start == std::string::npos) {
    return "";
  }
  const auto last = str.find_last_not_of(linear_whitespace);
  const auto length = last - start + 1;

  return str.substr(start, length);
}

template <typename T>
absl::StatusOr<T> StringToNumber(std::string_view str) {
  T value;
  auto fc_result = std::from_chars(str.data(), str.data() + str.size(), value);
  if (fc_result.ec != std::errc()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Expected a number, not ", str));
  }
  return value;
}

// Consider adding support for determining whether a particular header's value
// is a comma separated list, of which I know these headers:
//
//  Content-Encoding
//  Transfer-Encoding
//  TE
//  Accept
//  Accept-Charset
//  Accept-Encoding
//  Accept-Language
//  Trailer
//  Via
//  Connection
//  Upgrade
//  Content-Language
//  Vary
//  Allow
//  If-Match
//  If-None-Match
//  Accept-Ranges
//  Cache-Control
//  Pragma
//  Warning
//  WWW-Authenticate
//  Proxy-Authenticate
//
// A comma-separated list can be split into multiple header lines at the commas,
// i.e. replacing the comma with "\r\nHeader-Name:", and these are to be
// re-combined into a single list for processing.
//
// In addition, the Set-Cookie header is special in that it too allows multiple
// headers, but not as a comma separated list; each Set-Cookie header line deals
// with a single cookie.

}  // namespace

absl::StatusOr<HttpResponse> HttpResponse::Make(std::string response) {
  HttpResponse hr;

  // Split the headers and body apart by finding two EOLs in a row.
  const std::vector<std::string> headers_and_body = absl::StrSplit(
      response, absl::MaxSplits("\r\n\r\n", 1), absl::AllowEmpty());
  if (headers_and_body.size() != 2) {
    return absl::InvalidArgumentError("End of headers not found");
  }
  if (headers_and_body[0].empty()) {
    return absl::InvalidArgumentError("Headers not found");
  }
  hr.body_and_beyond = headers_and_body[1];

  // Split the headers into the status line and the header lines.
  const std::vector<std::string> status_and_headers = absl::StrSplit(
      headers_and_body[0], absl::MaxSplits("\r\n", 1), absl::AllowEmpty());
  if (status_and_headers.empty() || status_and_headers.size() > 2) {
    return absl::InvalidArgumentError("End of status line not found");
  }

  // Split the status line into the version, status code and optional message.
  const std::vector<std::string> status_parts = absl::StrSplit(
      status_and_headers[0], absl::MaxSplits(' ', 2), absl::AllowEmpty());
  if (status_parts.size() < 2 || status_parts.size() > 3) {
    return absl::InvalidArgumentError("Unable to split status line: " +
                                      status_and_headers[0]);
  }

  hr.http_version = status_parts[0];

  if (!absl::SimpleAtoi(status_parts[1], &hr.status_code)) {
    return absl::InvalidArgumentError(
        "Unable to parse status code as an integer: " + status_parts[1]);
  }

  if (status_parts.size() > 2) {
    hr.status_message = status_parts[2];
  }

  // Split each header line into a name and value.
  if (status_and_headers.size() > 1) {
    const std::vector<std::string> header_lines =
        absl::StrSplit(status_and_headers[1], "\r\n", absl::AllowEmpty());
    for (const auto& header_line : header_lines) {
      const std::vector<std::string> name_value = absl::StrSplit(
          header_line, absl::MaxSplits(':', 1), absl::AllowEmpty());
      if (name_value.size() != 2) {
        return absl::InvalidArgumentError("Unable to split header line: " +
                                          header_line);
      }
      hr.headers.insert(
          std::make_pair(name_value[0], TrimWhitespace(name_value[1])));
    }
  }

  if (!hr.HasHeaderValue("Content-Type", "application/json")) {
    return hr;
  }

  // Try decoding the body as JSON. If we fail, then it is an error on decoding
  // the whole response.
  ASSIGN_OR_RETURN(size_t content_length, hr.GetContentLength());
  if (content_length > hr.body_and_beyond.size()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Content-Length is ", content_length,
        ", but body_and_beyond.size() is only ", hr.body_and_beyond.size()));
  }

  std::string json_text = hr.body_and_beyond.substr(0, content_length);
  hr.body_and_beyond = hr.body_and_beyond.substr(content_length);

  ASSIGN_OR_RETURN(hr.json_value, JsonValue::Parse(json_text));

  return hr;
}

std::vector<std::string> HttpResponse::GetHeaderValues(
    const std::string& name) const {
  auto range = headers.equal_range(name);
  std::vector<std::string> values;
  for (auto iter = range.first; iter != range.second; ++iter) {
    values.push_back(iter->second);
  }
  return values;
}

absl::StatusOr<std::string> HttpResponse::GetSoleHeaderValue(
    const std::string& name) const {
  auto values = GetHeaderValues(name);
  if (values.empty()) {
    return absl::NotFoundError(absl::StrCat("Header '", name, "' not found"));
  } else if (values.size() > 1) {
    return absl::InvalidArgumentError(
        absl::StrCat("More than one '", name, "' header found"));
  }
  return values[0];
}

bool HttpResponse::HasHeader(const std::string& name) const {
  return headers.find(name) != headers.end();
}

bool HttpResponse::HasHeaderValue(const std::string& name,
                                  const std::string& value) const {
  auto range = headers.equal_range(name);
  for (auto iter = range.first; iter != range.second; ++iter) {
    if (iter->second == value) {
      return true;
    }
  }
  return false;
}

absl::StatusOr<size_t> HttpResponse::GetContentLength() const {
  ASSIGN_OR_RETURN(auto str, GetSoleHeaderValue("Content-Length"));
  return StringToNumber<size_t>(str);
}

}  // namespace test
}  // namespace alpaca

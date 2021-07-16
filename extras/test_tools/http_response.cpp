#include "extras/test_tools/http_response.h"

#include <string.h>

#include <string>

#include "absl/strings/str_split.h"

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
    return absl::FailedPreconditionError("End of headers not found");
  }
  if (headers_and_body[0].empty()) {
    return absl::FailedPreconditionError("Headers not found");
  }
  hr.body_and_beyond = headers_and_body[1];

  // Split the headers into the status line and the header lines.
  const std::vector<std::string> status_and_headers = absl::StrSplit(
      headers_and_body[0], absl::MaxSplits("\r\n", 1), absl::AllowEmpty());
  if (status_and_headers.empty() || status_and_headers.size() > 2) {
    return absl::FailedPreconditionError("End of status line not found");
  }

  // Split the status line into the version, status code and optional message.
  const std::vector<std::string> status_parts = absl::StrSplit(
      status_and_headers[0], absl::MaxSplits(' ', 2), absl::AllowEmpty());
  if (status_parts.size() < 2 || status_parts.size() > 3) {
    return absl::FailedPreconditionError("Unable to split status line: " +
                                         status_and_headers[0]);
  }

  hr.http_version = status_parts[0];

  if (!absl::SimpleAtoi(status_parts[1], &hr.status_code)) {
    return absl::FailedPreconditionError(
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
        return absl::FailedPreconditionError("Unable to split header line: " +
                                             header_line);
      }
      hr.headers.insert(
          std::make_pair(name_value[0], TrimWhitespace(name_value[1])));
    }
  }

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

bool HttpResponse::CaseInsensitiveLess::operator()(
    const std::string& lhs, const std::string& rhs) const {
  return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

}  // namespace test
}  // namespace alpaca

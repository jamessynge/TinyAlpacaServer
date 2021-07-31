#include "extras/test_tools/http_request.h"

#include <map>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "experimental/users/jamessynge/arduino/hostuino/extras/test_tools/case_insensitive_less.h"

namespace alpaca {
namespace test {
namespace {

std::string PercentEncodeString(const std::string& str) {
  std::string result;
  for (const char c : str) {
    if (' ' < c && c <= '~') {
      switch (c) {
        case '!':
        case '#':
        case '$':
        case '%':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case '/':
        case ';':
        case '=':
        case '?':
        case '@':
        case '[':
        case ']':
          break;

        default:
          // No special treatment needed.
          result.push_back(c);
          continue;
      }
    }
    // c needs to be percent encoded.
    result.push_back('%');
    const int hi = (static_cast<int>(c) >> 4) & 0xF;
    const int lo = static_cast<int>(c) & 0xF;
    result.push_back("0123456789ABCDEF"[hi]);
    result.push_back("0123456789ABCDEF"[lo]);
  }
  return result;
}

template <class M>
std::string PercentEncodePairs(const M& parameters) {
  auto str_formatter = [](std::string* out, const std::string& str) {
    absl::StrAppend(out, PercentEncodeString(str));
  };
  return absl::StrJoin(parameters, "&",
                       absl::PairFormatter(str_formatter, "=", str_formatter));
}

}  // namespace

HttpRequest::HttpRequest(const std::string& path) : path(path) {}

HttpRequest::HttpRequest(const std::string& method, const std::string& path)
    : method(method), path(path) {}

void HttpRequest::AddCommonParts() {
  AddCommonParameters();
  AddCommonHeaders();
}

// Adds headers such as Content-Type and Content-Length.
void HttpRequest::AddCommonHeaders() {
  AddHeaderIfUnset(
      "Accept",
      absl::StrJoin({"application/json", "text/plain", "text/html"}, ", "));
}

void HttpRequest::AddHeaderIfUnset(const std::string& name,
                                   const std::string& value) {
  headers.insert(std::make_pair(name, value));
}

void HttpRequest::SetHeader(const std::string& name, const std::string& value) {
  headers.insert_or_assign(name, value);
}

bool HttpRequest::HasHeader(const std::string& name) const {
  return headers.find(name) != headers.end();
}

// Adds parameters such as ClientID and ClientTransactionID.
void HttpRequest::AddCommonParameters() {
  AddParameterIfUnset("ClientID", "1");
  AddParameterIfUnset("ClientTransactionID", "2");
}

void HttpRequest::AddParameterIfUnset(const std::string& name,
                                      const std::string& value) {
  parameters.insert(std::make_pair(name, value));
}

void HttpRequest::SetParameter(const std::string& name,
                               const std::string& value) {
  parameters.insert_or_assign(name, value);
}

bool HttpRequest::HasParameter(const std::string& name) const {
  return parameters.find(name) != parameters.end();
}

absl::StatusOr<std::string> HttpRequest::GetParameter(
    const std::string& name) const {
  auto iter = parameters.find(name);
  if (iter == parameters.end()) {
    return absl::NotFoundError(name);
  }
  return iter->second;
}

// Produces the string to be send to the server.
std::string HttpRequest::ToString() {
  const char EOL[] = "\r\n";
  const auto parameters_string = PercentEncodePairs(parameters);
  std::string result = absl::StrCat(method, " ", path);
  result.reserve(256);
  if (!parameters_string.empty() && method == "GET") {
    result.push_back('?');
    result.append(parameters_string);
  }
  absl::StrAppend(&result, " ", version, EOL);
  if (method == "PUT") {
    if (parameters_string.empty()) {
      AddHeaderIfUnset("Content-Length", "0");
    } else {
      AddHeaderIfUnset("Content-Type", "application/x-www-form-urlencoded");
      AddHeaderIfUnset("Content-Length",
                       std::to_string(parameters_string.size()));
    }
  }
  for (const auto& elem : headers) {
    if (!elem.second.empty()) {
      absl::StrAppend(&result, elem.first, ": ", elem.second, EOL);
    }
  }
  absl::StrAppend(&result, EOL);
  if (method == "PUT") {
    result.append(parameters_string);
  }
  return result;
}

}  // namespace test
}  // namespace alpaca

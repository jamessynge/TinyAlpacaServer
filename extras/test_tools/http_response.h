#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_HTTP_RESPONSE_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_HTTP_RESPONSE_H_

// Provides a trivial parsing of HTTP responses. There is no validation that the
// strings make sense, only that the delimiters are in the right places.

#include <map>
#include <string>
#include <vector>

#include "absl/status/statusor.h"

namespace alpaca {
namespace test {

struct HttpResponse {
  struct CaseInsensitiveLess {
    bool operator()(const std::string& lhs, const std::string& rhs) const;
  };

  // Returns an HttpResponse based on the provided string, if it can be parsed
  // as such, else returns an error.
  static absl::StatusOr<HttpResponse> Make(std::string response);

  std::vector<std::string> GetHeaderValues(const std::string& name) const;
  bool HasHeader(const std::string& name) const;
  bool HasHeaderValue(const std::string& name, const std::string& value) const;

  std::string http_version;
  int status_code;
  std::string status_message;
  std::multimap<std::string, std::string, CaseInsensitiveLess> headers;

  // Everything after the headers, whether it is exactly the required size for
  // the body, or more or less for some reason.
  std::string body_and_beyond;
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_HTTP_RESPONSE_H_

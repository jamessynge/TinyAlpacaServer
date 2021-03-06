// TODO(jamessynge): Describe why this file exists/what it provides.

#include "http_response_header.h"

#include "constants.h"
#include "literals.h"
#include "utils/platform.h"

namespace alpaca {
namespace {
size_t WriteEolHeaderName(const Literal& name, Print& out) {
  size_t count = 0;
  count += Literals::HttpEndOfLine().printTo(out);
  count += name.printTo(out);
  count += out.print(':');
  count += out.print(' ');
  return count;
}
}  // namespace

HttpResponseHeader::HttpResponseHeader() { Reset(); }

void HttpResponseHeader::Reset() {
  status_code = EHttpStatusCode::kHttpInternalServerError;
  reason_phrase = Literal();
  content_type = {};
  content_length = 0;
}

size_t HttpResponseHeader::printTo(Print& out) const {
  size_t count = 0;
  count += Literals::HttpVersion().printTo(out);
  count += out.print(' ');
  count += out.print(static_cast<unsigned int>(status_code));
  count += out.print(' ');
  if (reason_phrase.size() > 0) {
    count += reason_phrase.printTo(out);
  }
  count += Literals::CommonResponseHeaders().printTo(out);

  count += WriteEolHeaderName(Literals::HttpContentType(), out);
  switch (content_type) {
    case EContentType::kApplicationJson:
      count += Literals::MimeTypeJson().printTo(out);
      break;

    case EContentType::kTextPlain:
      count += Literals::MimeTypeTextPlain().printTo(out);
      break;
  }
  count += WriteEolHeaderName(Literals::HttpContentLength(), out);
  count += out.print(content_length);
  count += Literals::HttpEndOfLine().printTo(out);

  // The end of an HTTP header is marked by a blank line.
  count += Literals::HttpEndOfLine().printTo(out);

  return count;
}

}  // namespace alpaca

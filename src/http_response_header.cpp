#include "http_response_header.h"

#include <McuCore.h>

#include "constants.h"
#include "literals.h"

namespace alpaca {
namespace {
size_t WriteEolHeaderName(const mcucore::ProgmemStringView& name, Print& out) {
  size_t count = 0;
  count += ProgmemStringViews::HttpEndOfLine().printTo(out);
  count += name.printTo(out);
  count += out.print(':');
  count += out.print(' ');
  return count;
}
}  // namespace

HttpResponseHeader::HttpResponseHeader() { Reset(); }

void HttpResponseHeader::Reset() {
  status_code = EHttpStatusCode::kHttpInternalServerError;
  reason_phrase = {};
  content_type = {};
  content_length = kContentLengthUnknown;
  do_close = true;
}

size_t HttpResponseHeader::printTo(Print& out) const {
  size_t count = 0;
  count += ProgmemStringViews::HttpVersion().printTo(out);
  count += out.print(' ');
  count += out.print(static_cast<unsigned int>(status_code));
  count += out.print(' ');  // Required, even if there is no reason phrase.
  if (!reason_phrase.empty()) {
    count += reason_phrase.printTo(out);
  }
  count += WriteEolHeaderName(ProgmemStringViews::Server(), out);
  count += ProgmemStringViews::TinyAlpacaServer().printTo(out);

  if (do_close || content_length == kContentLengthUnknown) {
    count += WriteEolHeaderName(ProgmemStringViews::Connection(), out);
    count += ProgmemStringViews::close().printTo(out);
  }

  count += WriteEolHeaderName(ProgmemStringViews::HttpContentType(), out);
  switch (content_type) {
    case EContentType::kApplicationJson:
      count += ProgmemStringViews::MimeTypeJson().printTo(out);
      break;

    case EContentType::kTextPlain:
      count += ProgmemStringViews::MimeTypeTextPlain().printTo(out);
      break;

    case EContentType::kTextHtml:
      count += ProgmemStringViews::MimeTypeTextHtml().printTo(out);
      break;
  }
  if (content_length != kContentLengthUnknown) {
    count += WriteEolHeaderName(ProgmemStringViews::HttpContentLength(), out);
    count += out.print(content_length);
  }

  // The end of an HTTP header is marked by a blank line.
  count += ProgmemStringViews::HttpEndOfLine().printTo(out);
  count += ProgmemStringViews::HttpEndOfLine().printTo(out);

  return count;
}

}  // namespace alpaca

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/any_string.h"

#include "utils/platform.h"

namespace alpaca {

AnyString::AnyString(const Literal& literal)
    : literal_(literal), is_literal_(true) {}

AnyString::AnyString(const StringView& view)
    : view_(view), is_literal_(false) {}

size_t AnyString::printTo(Print& out) const {
  if (is_literal_) {
    return literal_.printTo(out);
  } else {
    return view_.printTo(out);
  }
}
size_t AnyString::printJsonEscapedTo(Print& out) const {
  if (is_literal_) {
    return literal_.printJsonEscapedTo(out);
  } else {
    return view_.escaped().printTo(out);
  }
}

}  // namespace alpaca

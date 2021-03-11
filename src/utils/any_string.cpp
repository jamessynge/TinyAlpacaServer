// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/any_string.h"

#include "utils/platform.h"

namespace alpaca {

AnyString::AnyString(Literal literal)
    // std::move isn't available, so not using it.
    : literal_(literal), is_literal_(true) {}

AnyString::AnyString(StringView view)
    // std::move isn't available, so not using it.
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

size_t AnyString::size() const {
  if (is_literal_) {
    return literal_.size();
  } else {
    return view_.size();
  }
}

}  // namespace alpaca

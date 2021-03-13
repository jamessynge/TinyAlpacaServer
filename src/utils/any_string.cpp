#include "utils/any_string.h"

#include "utils/escaping.h"
#include "utils/platform.h"

namespace alpaca {
// std::move isn't available on Arduino, so not using it to move parameters.

AnyString::AnyString() : view_(StringView()), is_literal_(false) {}

AnyString::AnyString(Literal literal) : literal_(literal), is_literal_(true) {}

AnyString::AnyString(StringView view) : view_(view), is_literal_(false) {}

AnyString::AnyString(const AnyString& other) {
  is_literal_ = other.is_literal_;
  if (is_literal_) {
    literal_ = other.literal_;
  } else {
    view_ = other.view_;
  }
}

size_t AnyString::printTo(Print& out) const {
  if (is_literal_) {
    return literal_.printTo(out);
  } else {
    return view_.printTo(out);
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

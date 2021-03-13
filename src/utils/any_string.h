#ifndef TINY_ALPACA_SERVER_SRC_UTILS_ANY_STRING_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_ANY_STRING_H_

#include "utils/literal.h"
#include "utils/platform.h"
#include "utils/string_view.h"

namespace alpaca {

// AnyString allows either a Literal or a StringView to be output using the JSON
// encoder, and avoids the need to define two methods when one of the parameters
// is either a StringView or a Literal. It is essentially an adapter around a
// std::variant<Literal, StringView>, except std::variant isn't available on
// Arduino.
//
// TODO(jamessynge): Add support for TinyString here.
class AnyString : public Printable {
 public:
  // Deliberately NOT marked explicit so that either type may be used.
  AnyString();
  AnyString(Literal literal);  // NOLINT
  AnyString(StringView view);  // NOLINT

  AnyString(const AnyString& other);

  // Print the string to the provided Print instance.
  size_t printTo(Print& out) const override;

  // Returns the size in bytes (i.e. number of ASCII characters) in the string.
  size_t size() const;

 private:
  union {
    Literal literal_;
    StringView view_;
  };
  bool is_literal_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_ANY_STRING_H_

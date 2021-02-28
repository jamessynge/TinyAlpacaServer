#ifndef TINY_ALPACA_SERVER_SRC_ANY_STRING_H_
#define TINY_ALPACA_SERVER_SRC_ANY_STRING_H_

#include "literal.h"
#include "platform.h"
#include "string_view.h"

namespace alpaca {

// AnyString allows either a Literal or a StringView to be output using the JSON
// encoder, and avoids the need to define two methods where a parameter is some
// kind of string. It is essentially an adapter around a std::variant<Literal,
// StringView>, except std::variant isn't available on Arduino.
class AnyString : public Printable {
 public:
  // Deliberately NOT marked explicit so that either type may be used.
  AnyString(const Literal& literal);  // NOLINT
  AnyString(const StringView& view);  // NOLINT

  size_t printTo(Print& out) const override;
  size_t printJsonEscapedTo(Print& out) const;

  // #if TAS_HOST_TARGET
  //   // Support for tests and logging.
  //   friend std::ostream& operator<<(std::ostream& out,
  //                                   const Printable& printable);
  // #endif  // TAS_HOST_TARGET

 private:
  union {
    const Literal literal_;
    const StringView view_;
  };
  const bool is_literal_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ANY_STRING_H_

#ifndef TINY_ALPACA_SERVER_SRC_UTILS_ANY_PRINTABLE_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_ANY_PRINTABLE_H_

// AnyPrintable allows anything that we know can be passed to an overload of
// Print::print to be captured and passed around. This is in support of
// preparing responses to Alpaca requests by "concatenating" multiple printable
// values into a single Printable instance via PrintableStrCat.
//
// For those captured items that reference temporaries not contained by the
// capture, the item needs to outlive the AnyPrintable.

#include "extras/host_arduino/print.h"
#include "utils/any_string.h"
#include "utils/literal.h"
#include "utils/platform.h"
#include "utils/string_view.h"

namespace alpaca {

class AnyPrintable : public Printable {
  enum EFragmentType {
    kEmpty,
    kAnyString,
    kLiteral,
    kStringView,
    kPrintable,
    kChar,
    kInteger,
    kUnsignedInteger,
    kFloat,
    kDouble,
  };

 public:
  AnyPrintable();
  AnyPrintable(AnyString value);         // NOLINT
  AnyPrintable(Literal value);           // NOLINT
  AnyPrintable(StringView value);        // NOLINT
  AnyPrintable(Printable& value);        // NOLINT
  AnyPrintable(const Printable& value);  // NOLINT
  explicit AnyPrintable(char value);
  explicit AnyPrintable(int32_t value);
  explicit AnyPrintable(uint32_t value);
  explicit AnyPrintable(float value);
  explicit AnyPrintable(double value);
  // We can't copy a Printable instance, so we prevent temporaries from being
  // passed in.
  AnyPrintable(Printable&& value) = delete;
  AnyPrintable(const Printable&& value) = delete;

  // Copy and assignment operators.
  AnyPrintable(const AnyPrintable&);
  AnyPrintable& operator=(const AnyPrintable&);

  ~AnyPrintable() override;

  size_t printTo(Print& out) const override;

 private:
  EFragmentType type_;
  union {
    AnyString any_string_;
    Literal literal_;
    StringView view_;
    char char_;
    int32_t signed_;
    uint32_t unsigned_;
    float float_;
    double double_;
    const Printable* printable_;
  };
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_ANY_PRINTABLE_H_

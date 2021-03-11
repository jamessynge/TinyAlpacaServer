#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PRINTABLE_STR_CAT_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PRINTABLE_STR_CAT_H_

// PrintableStrCat provides the ability to create a Printable instance that
// concatenates multiple strings and numbers. The intent is to use this for
// creating error messages assembled from multiple parts. This is inspired by
// absl::StrCat, but does not perform allocation. Instead it addresses the fact
// that allocation is "hard" on an Arduino, except for stack variables.
// Therefore it holds a small array of PrintableFragment instances.
//
// Author: james.synge@gmail.com

#include "extras/host_arduino/print.h"
#include "utils/any_string.h"
#include "utils/literal.h"
#include "utils/platform.h"
#include "utils/string_view.h"

namespace alpaca {

class PrintableFragment : public Printable {
  enum EFragmentType {
    kEmpty,
    kLiteral,
    kStringView,
    kInteger,
    kUnsignedInteger,
    kFloat,
    kDouble,
    kPrintable,
  };

 public:
  PrintableFragment();
  PrintableFragment(Literal value);           // NOLINT
  PrintableFragment(StringView value);        // NOLINT
  PrintableFragment(int32_t value);           // NOLINT
  PrintableFragment(uint32_t value);          // NOLINT
  PrintableFragment(float value);             // NOLINT
  PrintableFragment(double value);            // NOLINT
  PrintableFragment(const Printable& value);  // NOLINT
  PrintableFragment(const PrintableFragment&) = default;

  PrintableFragment& operator=(const PrintableFragment&);

  size_t printTo(Print& out) const override;

 private:
  EFragmentType type_;
  union {
    Literal literal_;
    StringView view_;
    int32_t signed_;
    uint32_t unsigned_;
    float float_;
    double double_;
    const Printable* printable_;
  };
};

class PrintableStrCat : public Printable {
 public:
  explicit PrintableStrCat(PrintableFragment a) : size_(1), fragments_{a} {}
  PrintableStrCat(PrintableFragment a, PrintableFragment b)
      : size_(2), fragments_{a, b} {}
  PrintableStrCat(PrintableFragment a, PrintableFragment b, PrintableFragment c)
      : size_(3), fragments_{a, b, c} {}
  PrintableStrCat(PrintableFragment a, PrintableFragment b, PrintableFragment c,
                  PrintableFragment d)
      : size_(4), fragments_{a, b, c, d} {}
  PrintableStrCat(PrintableFragment a, PrintableFragment b, PrintableFragment c,
                  PrintableFragment d, PrintableFragment e)
      : size_(5), fragments_{a, b, c, d, e} {}

  size_t printTo(Print& out) const override;

 private:
  size_t size_;
  PrintableFragment fragments_[5];
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PRINTABLE_STR_CAT_H_

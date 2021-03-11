// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/printable_str_cat.h"

#include "utils/platform.h"

namespace alpaca {

PrintableFragment::PrintableFragment()
    : type_(PrintableFragment::kEmpty), signed_(0) {}

PrintableFragment::PrintableFragment(Literal value)
    : type_(PrintableFragment::kLiteral), literal_(value) {}

PrintableFragment::PrintableFragment(StringView value)
    : type_(PrintableFragment::kStringView), view_(value) {}

PrintableFragment::PrintableFragment(int32_t value)
    : type_(PrintableFragment::kInteger), signed_(value) {}

PrintableFragment::PrintableFragment(uint32_t value)
    : type_(PrintableFragment::kUnsignedInteger), unsigned_(value) {}

PrintableFragment::PrintableFragment(float value)
    : type_(PrintableFragment::kFloat), float_(value) {}

PrintableFragment::PrintableFragment(double value)
    : type_(PrintableFragment::kDouble), double_(value) {}

PrintableFragment::PrintableFragment(const Printable& value)
    : type_(PrintableFragment::kPrintable), printable_(&value) {}

PrintableFragment& PrintableFragment::operator=(
    const PrintableFragment& other) {
  type_ = other.type_;
  switch (type_) {
    case kEmpty:
      break;
    case kLiteral:
      literal_ = other.literal_;
      break;
    case kStringView:
      view_ = other.view_;
      break;
    case kInteger:
      signed_ = other.signed_;
      break;
    case kUnsignedInteger:
      unsigned_ = other.unsigned_;
      break;
    case kFloat:
      float_ = other.float_;
      break;
    case kDouble:
      double_ = other.double_;
      break;
    case kPrintable:
      printable_ = other.printable_;
      break;
  }

  return *this;
}

size_t PrintableFragment::printTo(Print& out) const {
  switch (type_) {
    case kEmpty:
      return 0;
    case kLiteral:
      return literal_.printTo(out);
    case kStringView:
      return view_.printTo(out);
    case kInteger:
      return out.print(signed_);
    case kUnsignedInteger:
      return out.print(unsigned_);
    case kFloat:
      return out.print(float_);
    case kDouble:
      return out.print(double_);
    case kPrintable:
      return printable_->printTo(out);
  }
}

size_t PrintableStrCat::printTo(Print& out) const {
  size_t count = 0;
  for (size_t ndx = 0; ndx < size_; ++ndx) {
    count += fragments_[ndx].printTo(out);
  }
  return count;
}

}  // namespace alpaca

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "literal_token.h"

namespace alpaca {
namespace internal {
bool ExactlyEqual(const Literal& literal, const StringView& view) {
  return literal == view;
}
bool CaseEqual(const Literal& literal, const StringView& view) {
  return literal.case_equal(view);
}
bool LoweredEqual(const Literal& literal, const StringView& view) {
  return literal.lowered_equal(view);
}
}  // namespace internal
}  // namespace alpaca

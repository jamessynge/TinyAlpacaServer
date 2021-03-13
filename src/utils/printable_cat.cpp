#include "utils/printable_cat.h"

#include "utils/array_view.h"

namespace alpaca {
namespace internal {

size_t PrintAnyPrintablesTo(const AnyPrintable* printables,
                            size_t num_printables, Print& out) {
  ArrayView<const AnyPrintable> view(printables, num_printables);
  size_t count = 0;
  for (const auto& elem : view) {
    count += elem.printTo(out);
  }
  return count;
}

}  // namespace internal
}  // namespace alpaca

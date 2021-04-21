#include "utils/print_misc.h"

#include "utils/literal.h"
#include "utils/platform.h"

namespace alpaca {

TAS_DEFINE_LITERAL(Undefined, "Undefined");

size_t PrintUnknownEnumValueTo(PrintableProgmemString name, uint32_t v,
                               Print& out) {
  size_t result = Undefined().printTo(out);
  result += name.printTo(out);
  result += out.print(' ');
  result += out.print('(');
  result += out.print(v);
  result += out.print(')');
  return result;
}

}  // namespace alpaca

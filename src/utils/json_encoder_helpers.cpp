#include "utils/json_encoder_helpers.h"

#include "utils/counting_print.h"
#include "utils/logging.h"
#include "utils/platform.h"

namespace alpaca {

size_t PrintableJsonObject::printTo(Print& out) const {
  CountingPrint counter(out);
  JsonObjectEncoder::Encode(source_, counter);
#if SIZE_MAX < UINT32_MAX
  TAS_DCHECK_LE(counter.count(), SIZE_MAX)
      << TAS_FLASHSTR("size_t max (") << SIZE_MAX
      << TAS_FLASHSTR(") is too small for ") << counter.count();
#endif
  return counter.count();
}

}  // namespace alpaca

#include "literals.h"

#include <McuCore.h>

namespace alpaca {

// Define static mcucore::ProgmemStringView factory methods in struct
// ProgmemStringViews, which acts as a nested namespace.
#define TAS_DEFINE_PROGMEM_LITERAL(name, literal)         \
  mcucore::ProgmemStringView ProgmemStringViews::name() { \
    return MCU_PSV_128(literal);                          \
  }

#include "literals.inc"

#undef TAS_DEFINE_PROGMEM_LITERAL

// Define static mcucore::ProgmemString factory methods in struct
// ProgmemStrings, which acts as a nested namespace.
#define TAS_DEFINE_PROGMEM_LITERAL(name, literal) \
  mcucore::ProgmemString ProgmemStrings::name() { return MCU_PSD_128(literal); }

#include "literals.inc"

#undef TAS_DEFINE_PROGMEM_LITERAL

}  // namespace alpaca

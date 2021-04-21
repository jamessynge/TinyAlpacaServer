
#include "device_types/cover_calibrator/cover_calibrator_constants.h"

#include "utils/inline_literal.h"
#include "utils/platform.h"
#include "utils/print_misc.h"

namespace alpaca {

PrintableProgmemString ToPrintableProgmemString(ECalibratorStatus v) {
  switch (v) {
    case ECalibratorStatus::kNotPresent:
      return TASLIT("NotPresent");
    case ECalibratorStatus::kOff:
      return TASLIT("Off");
    case ECalibratorStatus::kNotReady:
      return TASLIT("NotReady");
    case ECalibratorStatus::kReady:
      return TASLIT("Ready");
    case ECalibratorStatus::kUnknown:
      return TASLIT("Unknown");
    case ECalibratorStatus::kError:
      return TASLIT("Error");
  }
  return PrintableProgmemString();
}

size_t PrintValueTo(ECalibratorStatus v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("ECalibratorStatus"),
                                 static_cast<uint32_t>(v), out);
}

PrintableProgmemString ToPrintableProgmemString(ECoverStatus v) {
  switch (v) {
    case ECoverStatus::kNotPresent:
      return TASLIT("NotPresent");
    case ECoverStatus::kClosed:
      return TASLIT("Closed");
    case ECoverStatus::kMoving:
      return TASLIT("Moving");
    case ECoverStatus::kOpen:
      return TASLIT("Open");
    case ECoverStatus::kUnknown:
      return TASLIT("Unknown");
    case ECoverStatus::kError:
      return TASLIT("Error");
  }
  return PrintableProgmemString();
}

size_t PrintValueTo(ECoverStatus v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("ECoverStatus"),
                                 static_cast<uint32_t>(v), out);
}

// Define stuff

}  // namespace alpaca

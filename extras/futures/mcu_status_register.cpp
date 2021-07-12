#include "TinyAlpacaServer.h"

void logMCUStatusRegister(uint8_t mcusr) {
  alpaca::LogSink() << TAS_FLASHSTR("MCUSR: ") << alpaca::BaseHex << mcusr;
  if (TAS_VLOG_IS_ON(1)) {
    if (mcusr & _BV(JTRF)) {
      // JTAG Reset
      TAS_VLOG(1) << TAS_FLASHSTR("JTAG") << TASLIT(" reset occured");
    }
    if (mcusr & _BV(WDRF)) {
      // Watchdog Reset
      TAS_VLOG(1) << TAS_FLASHSTR("Watchdog") << TASLIT(" reset occured");
    }
    if (mcusr & _BV(BORF)) {
      // Brownout Reset
      TAS_VLOG(1) << TAS_FLASHSTR("Brownout") << TASLIT(" reset occured");
    }
    if (mcusr & _BV(EXTRF)) {
      // Reset button or otherwise some software reset
      TAS_VLOG(1) << TAS_FLASHSTR("External") << TASLIT(" reset occured");
    }
    if (mcusr & _BV(PORF)) {
      // Power On Reset
      TAS_VLOG(1) << TAS_FLASHSTR("Power-on") << TASLIT(" reset occured");
    }
  }
}

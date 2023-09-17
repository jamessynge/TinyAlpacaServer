#ifndef TINY_ALPACA_SERVER_EXTRAS_FUTURES_SETUP_AND_LOOP_H_
#define TINY_ALPACA_SERVER_EXTRAS_FUTURES_SETUP_AND_LOOP_H_

// Provides Setup and Loop functions that can be shared by many sketches running
// TinyAlpacaServer.
//
// NOTE: For now this is tightly tied to the Robodyn Mega ETH and the WIZnet
// W5500. Abstract that later... maybe.

#include <McuCore.h>
#include <McuNet.h>

#include "tiny_alpaca_server.h"

namespace alpaca {

class SetupAndLoop {
 public:
  explicit SetupAndLoop(TinyAlpacaDeviceServer& device_server);
  SetupAndLoop(TinyAlpacaDeviceServer& device_server,
               mcunet::OuiPrefix oui_prefix);

  void Setup();
  void Loop();

 private:
  TinyAlpacaDeviceServer& device_server_;
  TinyAlpacaNetworkServer network_server_;
  mcunet::IpDevice ip_device_;
  mcucore::EepromTlv eeprom_tlv_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_FUTURES_SETUP_AND_LOOP_H_

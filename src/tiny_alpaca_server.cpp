#include "tiny_alpaca_server.h"

namespace alpaca {

TinyAlpacaServer::TinyAlpacaServer(uint16_t tcp_port,
                                   const ServerDescription& server_description,
                                   ArrayView<DeviceInterfacePtr> devices)
    : alpaca_devices_(server_description, devices),
      server_connections_(alpaca_devices_, tcp_port),
      discovery_server_(tcp_port) {}

bool TinyAlpacaServer::Initialize() {
  return alpaca_devices_.Initialize() && server_connections_.Initialize() &&
         discovery_server_.Initialize();
}

void TinyAlpacaServer::PerformIO() {
  alpaca_devices_.MaintainDevices();
  server_connections_.PerformIO();
  discovery_server_.PerformIO();
}

}  // namespace alpaca

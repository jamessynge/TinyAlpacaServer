#include "tiny_alpaca_network_server.h"

#include <McuCore.h>

#include "alpaca_response.h"
#include "constants.h"
#include "http_response_header.h"
#include "literals.h"
#include "tiny_alpaca_device_server.h"

namespace alpaca {

TinyAlpacaNetworkServer::TinyAlpacaNetworkServer(
    TinyAlpacaDeviceServer& device_server, uint16_t tcp_port)
    : sockets_(tcp_port, device_server), discovery_server_(tcp_port) {}

bool TinyAlpacaNetworkServer::Initialize() {
  // Give everything a chance to initialize so that logs will contain relevant
  // info about any failures. Choosing to initialize the discovery server before
  // the sockets so that it occupies socket 0.
  bool result = discovery_server_.Initialize();
  if (!sockets_.Initialize()) {
    result = false;
  }
  return result;
}

void TinyAlpacaNetworkServer::PerformIO() {
  discovery_server_.PerformIO();
  sockets_.PerformIO();
}

}  // namespace alpaca

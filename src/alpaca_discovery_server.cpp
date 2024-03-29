#include "alpaca_discovery_server.h"

#include <McuCore.h>

namespace alpaca {
namespace {

constexpr auto kDiscoveryMessage = MCU_PSV("alpacadiscovery1");
constexpr uint16_t kAlpacaDiscoveryPort = 32227;

}  // namespace

bool TinyAlpacaDiscoveryServer::Initialize() {
  return udp_.begin(kAlpacaDiscoveryPort);
}

void TinyAlpacaDiscoveryServer::PerformIO() {
  int packet_size = udp_.parsePacket();
  if (packet_size == 0) {
    return;
  }

  MCU_VLOG(1) << MCU_PSD("Received UDP message of size ") << packet_size
              << MCU_PSD(" from ") << udp_.remoteIP() << ':'
              << udp_.remotePort();

  if (packet_size != kDiscoveryMessage.size()) {
    // Ignoring unexpected message.
    MCU_VLOG(1) << MCU_PSD("Ignoring UDP message of unexpected length");
    return;
  }

  // Read the packet into the buffer.
  char buffer[kDiscoveryMessage.size()];
  auto copied = udp_.read(buffer, packet_size);
  mcucore::StringView view(buffer, copied);

  MCU_VLOG(1) << MCU_PSD("UDP message contents: ") << mcucore::HexEscaped(view);

  if (copied != packet_size) {
    // Ignoring unexpected message.
    MCU_VLOG(1) << MCU_PSD("Expected to read ") << packet_size
                << MCU_PSD(" bytes, but actually got ") << copied;
    return;
  }

  // Got all the data.

  // Is the message the expected one?
  if (kDiscoveryMessage != view) {
    // Ignoring unexpected message.
    MCU_VLOG(1) << MCU_PSD("Received unexpected discovery message");
    return;
  }

  // Yes, looks like an ASCOM Alpaca discovery message. Tell the requestor the
  // TCP port on which we are listening for ASCOM Alpaca HTTP requests. Note
  // that we could implement a mcucore::JsonPropertySource to generate the JSON,
  // but it is so simple that it takes less Flash storage to generate it
  // explicitly.
  udp_.beginPacket(udp_.remoteIP(), udp_.remotePort());
  udp_.print(MCU_FLASHSTR(R"({"alpacaport": )"));
  udp_.print(tcp_port_, DEC);
  udp_.print('}');
  udp_.endPacket();
}

}  // namespace alpaca

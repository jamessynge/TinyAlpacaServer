#include "alpaca_discovery_server.h"

#include "utils/escaping.h"
#include "utils/literal.h"
#include "utils/platform.h"
#include "utils/string_compare.h"
#include "utils/string_view.h"

namespace alpaca {
namespace {
TAS_DEFINE_LITERAL(kAlpacaPortStart, R"({"alpacaport": )");
TAS_DEFINE_LITERAL(kDiscoveryMessage, "alpacadiscovery1");
constexpr uint16_t kAlpacaDiscoveryPort = 32227;
}  // namespace

bool TinyAlpacaDiscoveryServer::begin() {
  return udp_.begin(kAlpacaDiscoveryPort);
}

void TinyAlpacaDiscoveryServer::loop() {
  int packet_size = udp_.parsePacket();
  if (packet_size == 0) {
    return;
  }

  // DEBUGGING STUFF, COMMENT OUT LATER, OR USE Literals.
  TAS_DVLOG(1, "Received UDP message of size " << packet_size << " from "
                                               << udp_.remoteIP() << ":"
                                               << udp_.remotePort());

  if (packet_size == kDiscoveryMessage().size()) {
    // Ignoring unexpected message.
    TAS_DLOG(INFO, "Ignoring UDP message of unexpected length.");
    return;
  }

  // Read the packet into the buffer.
  char buffer[kDiscoveryMessage().size()];
  auto copied = udp_.read(buffer, packet_size);
  StringView view(buffer, copied);

  // TODO(jamessynge): If keeping this, then add a HexEscapedPrintable class to
  // escaping.h... plus support for streaming to Serial, of course.
  TAS_DVLOG(1, "UDP message contents: " << view);

  if (copied != packet_size) {
    // Ignoring unexpected message.
    TAS_LOG(WARNING, "Expected to read " << packet_size
                                         << " bytes, but actually got "
                                         << copied);
    return;
  }

  // Got all the data.

  // Is the message the expected one?
  if (kDiscoveryMessage() != view) {
    // Ignoring unexpected message.
    TAS_LOG(WARNING, "Received unexpected discovery message");
    return;
  }

  // Yes, looks like an ASCOM Alpaca discovery message. Tell the requestor the
  // TCP port on which we are listening for ASCOM Alpaca HTTP requests. Note
  // that we could implement a JsonPropertySource to generate the JSON, but it
  // is so simple that it takes less Flash storage to generate it explicitly.
  udp_.beginPacket(udp_.remoteIP(), udp_.remotePort());
  kAlpacaPortStart().printTo(udp_);
  udp_.print(tcp_port_, DEC);
  udp_.print('}');
  udp_.endPacket();
}

}  // namespace alpaca

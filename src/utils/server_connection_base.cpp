#include "utils/server_connection_base.h"

#include "utils/logging.h"
#include "utils/platform_ethernet.h"

namespace alpaca {
namespace {
class ExtendedEthernetClient : public EthernetClient {
 public:
  explicit ExtendedEthernetClient(uint8_t sock)
      : EthernetClient(sock), stopped_(false) {}

  void stop() override {
    stopped_ = true;
    EthernetClient::stop();
  }

  bool stopped() const { return stopped_; }

 private:
  bool stopped_;
};
}  // namespace

ServerConnectionBase::ServerConnectionBase(int sock_num, uint16_t tcp_port)
    : sock_num_(sock_num), tcp_port_(tcp_port), connected_(false) {}

ServerConnectionBase::~ServerConnectionBase() {}

bool ServerConnectionBase::BeginListening() {
  // We assume the socket is not in use, but fail in debug if we made a mistake.
  TAS_DCHECK(!connected_,
             "Why is socket " << sock_num_ << " already connected?");
  return DoListen();
}

bool ServerConnectionBase::DoListen() {
  connected_ = false;
  // TODO(jamessynge): Are there timeouts we should specify, e.g. for detecting
  // a client that doesn't ACK fast enough, or doesn't keep the connection
  // alive?
  return InitializeTcpListenerSocket(sock_num_, tcp_port_);
}

void ServerConnectionBase::PerformIO() {
  ExtendedEthernetClient client(sock_num_);

  if (!connected_) {
    if (!AcceptConnection(sock_num_)) {
      return;
    }
    connected_ = true;
    OnConnect(client);
  } else {
    // TODO(jamessynge): Detect connection timeout based disconnection (i.e. no
    // ACKs from peer for long enough).
    if (client.available() > 0) {
      OnCanRead(client);
    } else if (IsClientDone(sock_num_)) {
      // We've read all we can from the client, but the connection is still open
      // so we can still write.
      OnClientDone(client);
    }
  }
  if (client.stopped()) {
    DoListen();
  }
}

}  // namespace alpaca

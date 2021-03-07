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
    : sock_num_(sock_num), tcp_port_(tcp_port), was_connected_(false) {}

ServerConnectionBase::~ServerConnectionBase() {}

void ServerConnectionBase::BeginListening() {
  // We assume the socket is not in use, but complain in debug if we made a
  // mistake.
  TAS_DCHECK(!was_connected_,
             "Why is socket " << sock_num_ << " already connected?");

  InitializeTcpListenerSocket(sock_num_, tcp_port_);
  was_connected_ = false;
}

void ServerConnectionBase::PerformIO() {
  ExtendedEthernetClient client(sock_num_);

  if (!was_connected_) {
    if (client.connected()) {
      return;
    }
    OnConnect(client);
    if (client.stopped()) {
      InitializeTcpListenerSocket(sock_num_, tcp_port_);
      return;
    }
    was_connected_ = true;
  }

  if (IsClientDone(sock_num_)) {
    // We've read all we can from the client, but the connection is still open
    // so we can still write.
    OnClientDone(client);
  }

  if (client.available() > 0) {
    OnCanRead(client);
  }

  if (client.stopped()) {
    InitializeTcpListenerSocket(sock_num_, tcp_port_);
    was_connected_ = false;
    return;
  }
}

}  // namespace alpaca

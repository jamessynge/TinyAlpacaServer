#include "utils/server_connection_base.h"

#include "utils/logging.h"
#include "utils/platform_ethernet.h"

namespace alpaca {
namespace {
class ExtendedEthernetClient : public EthernetClient {
 public:
  explicit ExtendedEthernetClient(uint8_t sock)
      : EthernetClient(sock), stopped_(false) {
    TAS_VLOG(2) << TASLIT("ExtendedEthernetClient@0x") << this
                << TASLIT(" ctor");
  }
#if TAS_ENABLED_VLOG_LEVEL >= 2
  ~ExtendedEthernetClient() {
    TAS_VLOG(2) << TASLIT("ExtendedEthernetClient@0x") << this
                << TASLIT(" dtor");
  }
#endif

  void stop() override {
    TAS_VLOG(2) << TASLIT("ExtendedEthernetClient::stop, sock_num=")
                << getSocketNumber();
    stopped_ = true;
    EthernetClient::stop();
  }

  bool stopped() const {
    TAS_VLOG(2) << TASLIT("ExtendedEthernetClient::stopped, sock_num=")
                << getSocketNumber() << TASLIT(", returning ")
                << (stopped_ ? TASLIT("true") : TASLIT("false"));

    return stopped_;
  }

 private:
  bool stopped_;
};
}  // namespace

ServerConnectionBase::~ServerConnectionBase() {}

bool ServerConnectionBase::set_sock_num(uint8_t sock_num) {
  TAS_DCHECK(!has_socket()) << TASLIT("sock_num_: ") << sock_num_;
  if (has_socket()) {
    return false;
  }
  sock_num_ = sock_num;
  ExtendedEthernetClient client(sock_num_);
  OnConnect(client);
  if (client.stopped()) {
    sock_num_ = -1;
  }
  return true;
}

void ServerConnectionBase::PerformIO() {
  TAS_DCHECK(has_socket());
  if (!has_socket()) {
    return;
  }
  // Are we still connected?
  if (!PlatformEthernet::IsOpenForWriting(sock_num_)) {
    // Not anymore.
    OnDisconnect();
    sock_num_ = -1;
    return;
  }
  ExtendedEthernetClient client(sock_num_);
  if (client.available() > 0) {
    OnCanRead(client);
  } else if (PlatformEthernet::IsClientDone(sock_num_)) {
    // We've read all we can from the client, but the connection is still
    // open so we can still write.
    OnClientDone(client);
  }
  if (client.stopped()) {
    sock_num_ = -1;
  }
}

}  // namespace alpaca

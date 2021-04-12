#include "utils/server_connection_base.h"

#include "utils/logging.h"
#include "utils/platform_ethernet.h"

namespace alpaca {
namespace {
class ExtendedEthernetClient : public EthernetClient {
 public:
  explicit ExtendedEthernetClient(uint8_t sock)
      : EthernetClient(sock), stopped_(false) {
    TAS_VLOG(5) << TASLIT("ExtendedEthernetClient@0x") << this
                << TASLIT(" ctor");
  }
#if TAS_ENABLED_VLOG_LEVEL >= 2
  ~ExtendedEthernetClient() {
    TAS_VLOG(5) << TASLIT("ExtendedEthernetClient@0x") << this
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
#if TAS_ENABLED_VLOG_LEVEL >= 2
    if (stopped_) {
      TAS_VLOG(2) << TASLIT("ExtendedEthernetClient::stopped, sock_num=")
                  << getSocketNumber() << TASLIT(", returning ")
                  << (stopped_ ? TASLIT("true") : TASLIT("false"));
    }
#endif
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
    TAS_VLOG(2) << TASLIT("ServerConnectionBase::set_sock_num, sock_num=")
                << sock_num << ", OnConnect stopped the connection.";
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
    TAS_VLOG(2) << TASLIT("ServerConnectionBase::PerformIO, sock_num_=")
                << sock_num_ << ", calling OnDisconnect.";
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
    TAS_VLOG(2) << TASLIT("ServerConnectionBase::PerformIO, sock_num_=")
                << sock_num_ << ", calling OnClientDone.";
    OnClientDone(client);
  }
  if (client.stopped()) {
    TAS_VLOG(2) << TASLIT("ServerConnectionBase::PerformIO, sock_num_=")
                << sock_num_ << ", client.stop() was called.";
    sock_num_ = -1;
  }
}

}  // namespace alpaca

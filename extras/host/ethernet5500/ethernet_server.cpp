#include "extras/host/ethernet5500/ethernet_server.h"

#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "extras/host/ethernet5500/ethernet_class.h"
#include "extras/host/ethernet5500/host_sockets.h"
#include "extras/host/ethernet5500/w5500.h"
#include "logging.h"

EthernetServer::EthernetServer(uint16_t port) : port_(port) {}

void EthernetServer::begin() {
  VLOG(3) << "EthernetServer::begin entry";
  auto sock_num = alpaca::HostSockets::InitializeTcpListenerSocket(port_);
  if (sock_num >= 0) {
    EthernetClass::_server_port[sock_num] = port_;
  } else {
    LOG(WARNING) << "Unable to find a socket available for listening to port "
                 << port_;
  }
  VLOG(3) << "EthernetServer::begin exit";
}

void EthernetServer::accept() {
  VLOG(3) << "EthernetServer::accept() entry";
  // Not the same as Berkeley Socket API's ::accept(). It basically cleans up if
  // it finds that there are no sockets listening on port_ anymore. This is
  // needed because the W5500 sockets don't remember that they used to be
  // listening after a client connection is accepted.
  int listening = 0;
  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    EthernetClient client(sock);
    if (EthernetClass::_server_port[sock] == port_) {
      auto status = client.status();
      VLOG(3) << "EthernetServer::accept found socket " << sock
              << " listening to port " << port_ << " with status " << std::hex
              << (status + 0);
      if (status == SnSR::LISTEN) {
        listening = 1;
      } else if (status == SnSR::CLOSE_WAIT && !client.available()) {
        VLOG(3) << "Socket " << sock
                << " half closed and there is no more data available.";
        client.stop();
      }
    }
  }
  if (!listening) {
    VLOG(3) << "EthernetServer::accept no socket listening to port " << port_;
    begin();
  }
  VLOG(3) << "EthernetServer::accept() exit";
}

EthernetClient EthernetServer::available() {
  VLOG(3) << "EthernetServer::available() entry";
  accept();
  VLOG(3) << "EthernetServer::available() loop entry";
  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    EthernetClient client(sock);
    if (EthernetClass::_server_port[sock] == port_ &&
        (client.status() == SnSR::ESTABLISHED ||
         client.status() == SnSR::CLOSE_WAIT)) {
      VLOG(3) << "available: Socket " << sock
              << " is serving a connection to port " << port_;
      if (client.available()) {
        // XXX: don't always pick the lowest numbered socket.
        return client;
      }
    }
  }
  return EthernetClient(MAX_SOCK_NUM);
}

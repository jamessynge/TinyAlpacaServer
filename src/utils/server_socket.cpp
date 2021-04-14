#include "utils/server_socket.h"

#include <climits>
#include <cstdint>

#include "utils/logging.h"
#include "utils/platform.h"
#include "utils/platform_ethernet.h"

namespace alpaca {
namespace {

class TcpServerConnection : public WrappedClientConnection {
 public:
  explicit TcpServerConnection(EthernetClient &client,
                               ServerSocket::DisconnectData &disconnect_data)
      : client_(client), disconnect_data_(disconnect_data) {
    TAS_VLOG(5) << TASLIT("TcpServerConnection@0x") << this << TASLIT(" ctor");
    disconnect_data_.Reset();
  }
#if TAS_ENABLED_VLOG_LEVEL >= 2
  ~TcpServerConnection() {  // NOLINT
    TAS_VLOG(5) << TASLIT("TcpServerConnection@0x") << this << TASLIT(" dtor");
  }
#endif

  void close() override {
    auto sock_num = client_.getSocketNumber();
    auto status = PlatformEthernet::SocketStatus(sock_num);
    TAS_VLOG(2) << TASLIT("TcpServerConnection::close, sock_num=") << sock_num
                << TASLIT(", status=") << status;
    if (status == SnSR::ESTABLISHED || status == SnSR::CLOSE_WAIT) {
      disconnect_data_.RecordDisconnect();
      PlatformEthernet::DisconnectSocket(sock_num);
    }
  }

  bool connected() const override { return client_.connected(); }

  uint8_t getSocketNumber() const override { return client_.getSocketNumber(); }

 protected:
  Client &client() const override { return client_; }

 private:
  EthernetClient &client_;
  ServerSocket::DisconnectData &disconnect_data_;
};

MillisT ElapsedMillis(MillisT start_time) { return millis() - start_time; }

}  // namespace

ServerSocket::ServerSocket(uint16_t tcp_port, ServerSocketListener &listener)
    : sock_num_(MAX_SOCK_NUM),
      last_status_(0),
      listener_(listener),
      tcp_port_(tcp_port) {}
// ServerSocket::~ServerSocket() {}

bool ServerSocket::Initialize() {
  TAS_CHECK_EQ(sock_num_, MAX_SOCK_NUM)
      << "ServerSocket::Initialize already called, sock_num_=" << sock_num_;
  int sock_num = PlatformEthernet::FindUnusedSocket();
  if (0 <= sock_num && sock_num < MAX_SOCK_NUM) {
    sock_num_ = sock_num & 0xff;
    if (BeginListening(sock_num_)) {
      last_status_ = PlatformEthernet::SocketStatus(sock_num_);
      return true;
    }
    TAS_VLOG(1) << "listen for " << tcp_port_ << " failed with socket "
                << sock_num_;
    sock_num_ = MAX_SOCK_NUM;
  } else {
    TAS_VLOG(1) << "No free socket for " << tcp_port_;
  }
  return false;
}

#define LAST_STATUS_IS_UNEXPECTED_MESSAGE(expected_str, current_status) \
  TASLIT("Expected last_status_ to be ")                                \
      << TASLIT(expected_str) << TASLIT(", but is ") << last_status_    \
      << TASLIT("; current status is ") << current_status

#define LAST_STATUS_IS_UNEXPECTED(expected_str, current_status)        \
  TAS_DCHECK(false) << LAST_STATUS_IS_UNEXPECTED_MESSAGE(expected_str, \
                                                         current_status)

#define VERIFY_LAST_STATUS_IS(expected_status, current_status)            \
  TAS_DCHECK_EQ(last_status_, expected_status)                            \
      << "Expected last_status_ to be " << expected_status << ", but is " \
      << last_status_;

bool ServerSocket::BeginListening(uint8_t sock_num) {
  disconnect_data_.Reset();
  if (PlatformEthernet::InitializeTcpListenerSocket(sock_num, tcp_port_)) {
    last_status_ = PlatformEthernet::SocketStatus(sock_num);
    VERIFY_LAST_STATUS_IS(SnSR::LISTEN, last_status_);
    return true;
  }
  TAS_VLOG(1) << "listen for " << tcp_port_ << " failed with socket "
              << sock_num;
  return false;
}

// Notifies listener_ of relevant events/states of the socket (i.e. a new
// connection from a client, available data to read, room to write, client
// disconnect). The current implementation will make at most one of the
// On<Event> calls per call to PerformIO. This method is expected to be called
// from the loop() function of an Arduino sketch (i.e. typically hundreds or
// thousands of times a second).
void ServerSocket::PerformIO() {
  // Detect transitions.
  auto status = PlatformEthernet::SocketStatus(sock_num_);
  if (status == last_status_) {
    // No change.
    if (status == SnSR::ESTABLISHED || status == SnSR::CLOSE_WAIT) {
      AnnounceCanRead();
      return;
    }
    // Should we return here? Maybe... but not if waiting for a timeout on a
    // disconnect.
  }

  switch (status) {
    case SnSR::CLOSED:
      BeginListening(sock_num_);
      return;

    case SnSR::LISTEN:
      VERIFY_LAST_STATUS_IS(SnSR::LISTEN, status);
      break;

    case SnSR::SYNRECV:
      // This is a transient state that the chip handles (i.e. responds with a
      // SYN/ACK, waits for an ACK from the client to complete the three step
      // TCP handshake).
#if TAS_ENABLED_VLOG_LEVEL > 0
      if (!(last_status_ == SnSR::LISTEN || last_status_ == SnSR::SYNRECV)) {
        LAST_STATUS_IS_UNEXPECTED("LISTEN or SYNRECV", status);
      }
#endif
      break;

    case SnSR::ESTABLISHED:
#if TAS_ENABLED_VLOG_LEVEL > 0
      if (!(last_status_ == SnSR::LISTEN || last_status_ == SnSR::SYNRECV)) {
        LAST_STATUS_IS_UNEXPECTED("LISTEN or SYNRECV", status);
      }
#endif
      last_status_ = status;
      AnnounceConnected();
      return;

    case SnSR::CLOSE_WAIT:
      // Do NOT update last_status_, leave that decision to HandleCloseWait.
      HandleCloseWait();
      return;

    case SnSR::FIN_WAIT:
    case SnSR::CLOSING:
    case SnSR::TIME_WAIT:
    case SnSR::LAST_ACK:
      // Transient states after the connection is closed.
      if (last_status_ == SnSR::ESTABLISHED ||
          last_status_ == SnSR::CLOSE_WAIT) {
        disconnect_data_.RecordDisconnect();
        listener_.OnDisconnect();
        return;
      } else {
        // The Ethernet3 library baked in a limit of 1 second for closing a
        // connection, and did so by using a loop checking to see if the
        // connection closed. Since this implementation doesn't block in a loop,
        // we can allow a bit more time.
        constexpr MillisT kDisconnectMaxMillis = 5000;
        if (disconnect_data_.disconnected &&
            disconnect_data_.ElapsedDisconnectTime() > kDisconnectMaxMillis) {
          // Time to give up.
          PlatformEthernet::CloseSocket(sock_num_);
        }
      }
      break;

    // States that the socket should not have if it is being used as a server
    // socket.
    case SnSR::INIT:  // Transient state during setup of a TCP listener, not
                      // visible yet.
    case SnSR::SYNSENT:
    case SnSR::UDP:
    case SnSR::IPRAW:
    case SnSR::MACRAW:
    case SnSR::PPPOE:
      TAS_DCHECK(false) << TASLIT("Socket ") << sock_num_
                        << TASLIT(" has unexpected status ") << status
                        << TASLIT(", last_status_ is ") << last_status_;
      break;

    default:
      TAS_DCHECK(false) << TASLIT("Socket ") << sock_num_
                        << TASLIT(" has UNKNOWN status ") << status
                        << TASLIT(", last_status_ is ") << last_status_;
  }
  last_status_ = status;
}

// NOTE: Could choose to add another method that accepts a member function
// pointer to one of the SocketListener methods, and then delegate from the
// AnnounceX methods to that method. It may be worth doing if it notably reduces
// flash consumption.

void ServerSocket::AnnounceConnected() {
  EthernetClient client(sock_num_);
  TcpServerConnection conn(client, disconnect_data_);
  listener_.OnConnect(conn);
  HandleConnectionClose();
}

void ServerSocket::AnnounceCanRead() {
  EthernetClient client(sock_num_);
  TcpServerConnection conn(client, disconnect_data_);
  listener_.OnCanRead(conn);
  HandleConnectionClose();
}

void ServerSocket::HandleCloseWait() {
  EthernetClient client(sock_num_);
  TcpServerConnection conn(client, disconnect_data_);
  if (client.available() > 0) {
    // Still have data that we can read from the client (i.e. buffered up in the
    // network chip).
    // TODO(jamessynge): Determine whether we get the CLOSE_WAIT state before
    // we've read all the data from the client, or only once we've drained those
    // buffers.
    listener_.OnCanRead(conn);
  } else {
    last_status_ = SnSR::CLOSE_WAIT;
    listener_.OnHalfClosed(conn);
  }
  HandleConnectionClose();
}

void ServerSocket::HandleConnectionClose() {
  if (disconnect_data_.disconnected) {
    last_status_ = PlatformEthernet::SocketStatus(sock_num_);
  }
}

void ServerSocket::DisconnectData::RecordDisconnect() {
  if (!disconnected) {
    disconnected = true;
    disconnect_time_millis = millis();
  }
}

void ServerSocket::DisconnectData::Reset() {
  disconnected = false;
  disconnect_time_millis = 0;
}

MillisT ServerSocket::DisconnectData::ElapsedDisconnectTime() {
  TAS_DCHECK(disconnected);
  return ElapsedMillis(disconnect_time_millis);
}

}  // namespace alpaca

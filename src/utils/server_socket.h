#ifndef TINY_ALPACA_SERVER_SRC_UTILS_SERVER_SOCKET_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_SERVER_SOCKET_H_

// ServerSocket binds a hardware socket of a WIZnet W5500 to listen for
// connections to a TCP port, and dispatches the handling of connections
// received by that socket to a listener. The binding starts when Initialize is
// called and lasts from then on (i.e. there is no teardown support, as that
// isn't needed in the embedded setting). After a connection is closed, the
// socket will resume listening for new connections.
//
// Author: james.synge@gmail.com

#include "utils/connection.h"
#include "utils/platform.h"
#include "utils/socket_listener.h"

namespace alpaca {

class ServerSocket {
 public:
  // Struct used to record whether the listener called Connection::close(), and
  // if so, when.
  struct DisconnectData {
    void RecordDisconnect();
    void Reset();
    // Time since RecordDisconnect set disconnect_time_millis.
    MillisT ElapsedDisconnectTime();

    bool disconnected{false};
    MillisT disconnect_time_millis{0};
  };

  ServerSocket(uint16_t tcp_port, ServerSocketListener& listener);

  // Finds a closed hardware socket and starts listening for TCP connections to
  // 'tcp_port'. Returns true if able to find such a socket and configure the
  // underlying socket for listening. Returns false if already successfully
  // called.
  bool PickClosedSocket();

  // Returns true if has a hardware socket,
  bool HasSocket() const;

  // Returns true if the hardware socket is somewhere between LISTENING and
  // CLOSED w.r.t. the TCP connection lifecyle. This may include just starting
  // the TCP handshake, or waiting for timeout following the closure of the
  // connection.
  bool IsConnected() const;

  // Notifies listener_ of relevant events/states of the socket (i.e. a new
  // connection from a client, available data to read, room to write, client
  // disconnect). The current implementation will make at most one of the
  // On<Event> calls per call to PerformIO. This method is expected to be called
  // from the loop() function of an Arduino sketch (i.e. typically hundreds or
  // thousands of times a second).
  void PerformIO();

  // Release the socket; if IsConnected is true does nothing and returns false;
  // if not HasSocket, returns false (and fails a DCHECK in debug builds).
  bool ReleaseSocket();

 private:
  enum class EConnectionStatus : uint8_t {
    kNone,
    kConnected,
    kHalfClosed,
    kClosing,
  };

  // May already be listening, but not worrying about that.
  bool BeginListening();

  void AnnounceConnected();
  void AnnounceCanRead();
  void HandleCloseWait();

  void MaybeAnnounceDisconnect();

  // If the listener called Connection::close(), we'll handle that by performing
  // a disconnect and recording the time when it started. That allows us to
  // safely close the connection after a suitable timeout, and without blocking
  // (EthernetClient::close() is blocking).
  void DetectDisconnect();

  // Handle the transition from closing to closed to listening.
  // If kClosing, and the time to complete the Close has taken to
  void MaybeHandleClosure();

  // If sock_num_ is >= MAX_SOCK_NUM, then there isn't (yet) a hardware socket
  // bound to this ServerSocket instance.
  uint8_t sock_num_;

  // Status at the end of the last call to Initialize or PerformIO.
  // Status after a successful Initialize call will be SnSR::LISTEN,
  uint8_t last_status_;

  // Status of the connection as last reported to the listener; starts as kNone.
  EConnectionStatus prev_status_;

  // Object to be called with events.
  ServerSocketListener& listener_;

  // The TCP port to listen on.
  const uint16_t tcp_port_;

  // The time when we initiated or discovered a disconnect of a connection.
  DisconnectData disconnect_data_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_SERVER_SOCKET_H_

#ifndef TINY_ALPACA_SERVER_SRC_UTILS_SERVER_SOCKET_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_SERVER_SOCKET_H_

// ServerSocket binds a hardware socket of a WIZnet W5500 to listen for
// connections to a TCP port, and dispatches the handling of connections
// received by that socket to a listener. The binding starts when
// PickClosedSocket is called and lasts from then until ReleaseSocket is called.
//
// The socket status value is used to drive the behavior of instances that have
// a socket when PerformIO is called:
//
// * If there was an open connection (ESTABLISHED or CLOSE_WAIT) and is now not
//   open, then the listener's OnDisconnect method is called.
//
// * If the socket is closed, then we start listening.
//
// * If the connection transitions from LISTENING to ESTABLISHED or CLOSE_WAIT,
//   we call the listeners OnConnect method (via AnnounceConnected).
//
// * If the connection is ESTABLISHED, we call the listeners OnCanRead.
//
// * If the connection is CLOSE_WAIT, we call HandleCloseWait.
//
// * If the connection is closing (e.g. PlatformEthernet::StatusIsClosing), we
//   call HandleClosingTimeout to ensure the socket isn't in the state for too
//   long.
//
// * If the connection is in some other state (e.g. MACRAW), we DCHECK and then
//   close the hardware socket. On the next call to PerformIO we'll start
//   listening again.
//
// last_status_ is used to detect transitions, and is updated at selected times
// so that we don't miss key transitions.
//
// Author: james.synge@gmail.com

#include "experimental/users/jamessynge/arduino/mcucore/src/mcucore_platform.h"
#include "utils/connection.h"
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

    bool disconnected;
    MillisT disconnect_time_millis;
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
  // May already be listening, but not worrying about that.
  bool BeginListening();

  // Announce a new connection to the listener.
  void AnnounceConnected();

  // Give the listener a chance to read from (or write to) the connection.
  void AnnounceCanRead();

  // Give the listener a chance to write to a half-closed connection, or to read
  // from it if there is still buffered input.
  void HandleCloseWait();

  // If the listener called Connection::close(), we'll handle that by performing
  // a disconnect and recording the time when it started. That allows us to
  // safely close the connection after a suitable timeout, and without blocking
  // (EthernetClient::close() is blocking).
  void DetectListenerInitiatedDisconnect();

  // Detect when a closing connection has taken too long to be cleaned up.
  void DetectCloseTimeout();

  // Close the hardware socket, and inform the listener of the disconnect if
  // we've informed it of a connection but it hasn't initiated or been notified
  // of the closure of that connection.
  void CloseHardwareSocket();

  // If sock_num_ is >= MAX_SOCK_NUM, then there isn't (yet) a hardware socket
  // bound to this ServerSocket instance.
  uint8_t sock_num_;

  // Status at the end of the last call to Initialize or PerformIO.
  // Status after a successful Initialize call will be SnSR::LISTEN,
  uint8_t last_status_;

  // Object to be called with events.
  ServerSocketListener& listener_;

  // The TCP port to listen on.
  const uint16_t tcp_port_;

  // The time when we initiated or discovered a disconnect of a connection.
  DisconnectData disconnect_data_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_SERVER_SOCKET_H_

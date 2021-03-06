#ifndef TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_
#define TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_

// TinyAlpacaServer is a wrapper around sstaub's Ethernet3 library, replacing
// Ethernet3's EthernetServer library.
//
// We make the following assumptions:
//
// * TinyAlpacaServer (TAS) owns all of the sockets provided by the Ethernet
//   instance (e.g. up to 8 in the case of a WIZ5500), and no other TCP server
//   is attempting to share that pool.
//
// * TAS.loop() is called by the Arduino defined loop method ~every time.
//
// * TAS takes care of calling the DHCP code to maintain the DHCP lease.
//
// * TAS dispatches TCP connection creation, input data, output space and
//   connection deletion events to the appropriate handler (e.g. the request
//   decoder).
//
// * When a complete request has been decoded, TAS dispatches the request to
//   the appropriate handler.
//
// * TAS handles the UDP packets of the Alpaca discovery protocol using data
//   provided at startup (e.g. the set of attached devices).

#include "utils/platform.h"

namespace alpaca {

class TinyAlpacaServer {
 public:
  TinyAlpacaServer(/*static data to configure the server*/) {}

  // Initializes the network chip, prepares to receive TCP connections and UDP
  // packets. Returns true if able to setup the network chip, even if the
  // network cable is not currently connected or DHCP fails. Returns false if
  // there is a fundamental problem (e.g. no network chip).
  bool begin();

  // Performs network IO as appropriate.
  void loop();

  static uint32_t GetNextServerTransactionId();

 private:
  static uint32_t server_transaction_id_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_

#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET3_ETHERNET_UDP_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET3_ETHERNET_UDP_H_

// Incomplete declaration/implementation of Ethernet3's EthernetUDP class.

#include "extras/host/arduino/ip_address.h"
#include "extras/host/arduino/stream.h"
#include "extras/host/ethernet3/ethernet_client.h"
#include "extras/host/ethernet3/ethernet_config.h"

class EthernetUDP : public Stream {
 public:
  // Initialize, start listening on specified port. Returns 1 if
  // successful, 0 if there are no sockets available to use
  virtual uint8_t begin(uint16_t udp_port);

  // Sending UDP packets

  // Start building up a packet to send to the remote host specific in ip and
  // port Returns 1 if successful, 0 if there was a problem with the supplied IP
  // address or port
  virtual int beginPacket(IPAddress ip, uint16_t port) { return 0; }

  // Start building up a packet to send to the remote host specific in host and
  // port Returns 1 if successful, 0 if there was a problem resolving the
  // hostname or port
  virtual int beginPacket(const char* host, uint16_t port) { return 0; }

  // Finish off this packet and send it
  // Returns 1 if the packet was sent successfully, 0 if there was an error
  virtual int endPacket() { return 0; }
  // Write a single byte into the packet
  size_t write(uint8_t) override { return 0; }
  // Write size bytes from buffer into the packet
  size_t write(const uint8_t* buffer, size_t size) override { return 0; }

  using Stream::write;

  // Start processing the next available incoming packet
  // Returns the size of the packet in bytes, or 0 if no packets are available
  virtual int parsePacket() { return 0; }

  // Number of bytes remaining in the current packet
  int available() override { return 0; }

  // Read a single byte from the current packet
  int read() override { return -1; }

  // Read up to len bytes from the current packet and place them into buffer
  // Returns the number of bytes read, or 0 if none are available
  virtual int read(uint8_t* buffer, size_t len) { return 0; }

  // Read up to len characters from the current packet and place them into
  // buffer Returns the number of characters read, or 0 if none are available
  virtual int read(char* buffer, size_t len) {
    return read(reinterpret_cast<uint8_t*>(buffer), len);
  }

  // Return the next byte from the current packet without moving on to the next
  // byte
  int peek() override { return 0; }

  // Finish reading the current packet.
  void flush() override {}

  // Return the IP address of the host who sent the current incoming packet
  virtual IPAddress remoteIP() { return IPAddress(); }

  virtual void remoteIP(uint8_t* ip) {}

  // Return the port of the host who sent the current incoming packet
  virtual uint16_t remotePort() { return 0; }

  // Return the MAC address of the host who sent the current incoming packet
  virtual void remoteMAC(uint8_t* mac) {}
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET3_ETHERNET_UDP_H_

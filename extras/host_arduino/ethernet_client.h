#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_ETHERNET_CLIENT_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_ETHERNET_CLIENT_H_

// TODO(jamessynge): Describe why this file exists/what it provides.
#include "extras/host_arduino/client.h"

class EthernetClient : public Client {
  //  public:
  //   EthernetClient() : sockindex(MAX_SOCK_NUM), _timeout(1000) {}
  //   EthernetClient(uint8_t s) : sockindex(s), _timeout(1000) {}

  //   uint8_t status();
  //   virtual int connect(IPAddress ip, uint16_t port);
  //   virtual int connect(const char *host, uint16_t port);
  //   virtual int availableForWrite(void);
  //   virtual size_t write(uint8_t);
  //   virtual size_t write(const uint8_t *buf, size_t size);
  //   virtual int available();
  //   virtual int read();
  //   virtual int read(uint8_t *buf, size_t size);
  //   virtual int peek();
  //   virtual void flush();
  //   virtual void stop();
  //   virtual uint8_t connected();
  //   virtual operator bool() { return sockindex < MAX_SOCK_NUM; }
  //   virtual bool operator==(const bool value) { return bool() == value; }
  //   virtual bool operator!=(const bool value) { return bool() != value; }
  //   virtual bool operator==(const EthernetClient &);
  //   virtual bool operator!=(const EthernetClient &rhs) {
  //     return !this->operator==(rhs);
  //   }
  //   uint8_t getSocketNumber() const { return sockindex; }
  //   virtual uint16_t localPort();
  //   virtual IPAddress remoteIP();
  //   virtual uint16_t remotePort();
  //   virtual void setConnectionTimeout(uint16_t timeout) { _timeout = timeout;
  //   }

  //   friend class EthernetServer;

  //   using Print::write;

  //  private:
  //   uint8_t sockindex;  // MAX_SOCK_NUM means client not in use
  //   uint16_t _timeout;
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_ETHERNET_CLIENT_H_

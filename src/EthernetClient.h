#ifndef ethernetclient_h
#define ethernetclient_h

#include "utility/w5500.h"
#include <Print.h>
#include <Client.h>
#include <IPAddress.h>

class EthernetClient : public Client {
private:
  static uint16_t _srcport;
  uint8_t _sock;

public:
  EthernetClient();
  EthernetClient(uint8_t sock);

  uint8_t status();
  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char *host, uint16_t port);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek();
  virtual void flush();
  virtual void stop();
  virtual uint8_t connected();
  virtual operator bool();
  virtual bool operator==(const bool value) { return bool() == value; }
  virtual bool operator!=(const bool value) { return bool() != value; }
  virtual bool operator==(const EthernetClient&);
  virtual bool operator!=(const EthernetClient& rhs) { return !this->operator==(rhs); };
  uint8_t getSocketNumber();

  // Return the IP address of the host who sent the current incoming packet
  virtual void remoteIP(uint8_t *ip);
  // Return the MAC address of the host who sent the current incoming packet
  virtual void remoteMAC(uint8_t *mac);

  // get the value of the socket mode register
  virtual uint8_t getSocketMode();
  // set the 'no Delayed ACK' option
  virtual void setNoDelayedACK(bool ack = false);
  // get the state of 'no Delayed ACK' option
  virtual bool getNoDelayedACK();

  virtual void clearInterrupt();
  virtual void enableInterrupt();
  virtual void disableInterrupt();
  virtual uint8_t readInterrupt();

  friend class EthernetServer;
  
  using Print::write;


};

#endif

#ifndef ntpunixtime_h
#define ntpunixtime_h

class EthernetUDP;

#include "Arduino.h"
#include "EthernetUdp3.h"

class UnixTime {
    private:
        EthernetUDP *_ntp;
        const uint16_t _port=123;
        uint8_t _udpInited;
        uint8_t _pktLen;               // received packet length
        const uint8_t _pollIntv = 150; // poll every this many ms
    public:
        UnixTime(EthernetUDP *ntp);
        UnixTime(EthernetUDP &ntp);
        IPAddress timeServer;
        void stop();
        void begin();
        void requestUnixTime();
        uint32_t returnUnixTime();
        uint32_t getUnixTime();
};

#endif


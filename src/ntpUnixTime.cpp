/*
 * © Francesco Potortì 2013 - GPLv3 - Revision: 1.13
 *
 * Send an NTP packet and wait for the response, return the Unix time
 *
 * To lower the memory footprint, no buffers are allocated for sending
 * and receiving the NTP packets.  Four bytes of memory are allocated
 * for transmision, the rest is random garbage collected from the data
 * memory segment, and the received packet is read one byte at a time.
 * The Unix time is returned, that is, seconds from 1970-01-01T00:00.
 *
 * Modified by Cal Abel 2018
 */

#include "ntpUnixTime.h"

UnixTime::UnixTime(EthernetUDP *ntp){
    _ntp = ntp;
}

void UnixTime::begin(){
    _ntp->stop(); // if not already done close the open port
    _udpInited = _ntp->begin(_port); // open socket on specified port
}

void UnixTime::stop(){
    _ntp->stop();
}

void UnixTime::requestUnixTime(){
        // Only the first four bytes of an outgoing NTP packet need to be set
    // appropriately, the rest can be unspecified.
    // https://www.cisco.com/c/en/us/about/press/internet-protocol-journal/back-issues/table-contents-58/154-ntp.html
    // EC Peer Clock Precision: 0.000001 sec
    // 06 Peer Polling Interval: 6 (64 sec)
    // 03 Peer Clock Stratum: secondary reference (4)
    //      LI    VN  Mode
    // E3 0b11 0b100 0b011
    const uint32_t ntpFirstFourBytes = 0xEC0604E3; // NTP request header
    
    // Fail if EthernetUDP.begin() could not init a socket
    if (!_udpInited)
        return;

    // Clear received data from possible stray received packets
    _ntp->flush();

    // Send an NTP request
    if (! (_ntp->beginPacket(timeServer, _port)
    	&& _ntp->write((uint8_t *)&ntpFirstFourBytes, 48) == 48
        && _ntp->endPacket()))
        return;                 // sending request failed 
}

uint32_t UnixTime::returnUnixTime(){
    uint8_t pollIntv = _pollIntv/2;
    if (_pktLen != 48) {
        (_pktLen = _ntp->parsePacket()) == 48; // if not previously called parse the packet
        pollIntv = 10;  // adjust the poll interval for an interrupt driven response 
    }
    if (_pktLen != 48)
        return 0;                              // no correct packet received

    // Read and discard the first useless bytes
    // Set useless to 32 for speed; set to 40 for accuracy.
    const uint8_t useless = 40;
    for (uint8_t i = 0; i < useless; ++i)
        _ntp->read();

    // Read the integer part of sending time
    uint32_t time = _ntp->read(); // NTP time
    for (uint8_t i = 1; i < 4; i++)
        time = time << 8 | _ntp->read();

    // Round to the nearest second if we want accuracy
    // The fractionary part is the next byte divided by 256: if it is
    // greater than 500ms we round to the next second; we also account
    // for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
    // additionally, we account for how much we delayed reading the packet
    // since its arrival, which we assume on average to be _pollIntv/2.
    time += (_ntp->read() > 115 - pollIntv);

    // Discard the rest of the packet
    _ntp->flush();

    return time - 2208988800ul;      // convert NTP time to Unix time
}

uint32_t UnixTime::getUnixTime() {
    // provided for non task scheduler environments
    UnixTime::requestUnixTime();

    // Wait for response; check every pollIntv ms up to maxPoll times
    const uint8_t maxPoll = 15;   // poll up to this many times
    for (uint8_t i=0; i<maxPoll; i++) {
        if ((_pktLen = _ntp->parsePacket()) == 48)
            break;
        delay(_pollIntv);
    }
    
    return UnixTime::returnUnixTime();
}
//
//  Serial.hpp
//  testSerial
//
//  Created by Ryan Homer on 2021-07-24.
//

#ifndef Serial_hpp
#define Serial_hpp

#include <stdint.h>
#include <termios.h>
#include <string>

using namespace std;

class Serial {
private:
    int            _fd = -1; // file descriptor
    speed_t        _baudrate = B9600;
    unsigned short _stopBits = 1;
    unsigned short _bitSize = CS8;

    inline static const uint16_t _bufSz = 256;
    char           _buf[_bufSz];

public:
    string port;

    // bit flags
    uint8_t        parity:1 = false;

private:
    static void updateTtyFlag(tcflag_t& flag, int mask, bool set) {
        if (set) flag |= mask;
        else flag &= ~mask;
    }
    static void setTtyFlagBits(tcflag_t& flag, int mask) { updateTtyFlag(flag, mask, true); }
    static void clearTtyFlagBits(tcflag_t& flag, int mask) { updateTtyFlag(flag, mask, false); }
    static unsigned short mapBitSizeToMask(unsigned short bitSize);
    void updateTty(struct termios& tty);

public:
    Serial() {};
    Serial(const string &port,
           speed_t baudrate = B9600,
           bool parity = false,
           unsigned short stopBits = 1,
           unsigned short bitSize = 8);

    // Don't allow copies to be made
    Serial(const Serial&) = delete;
    Serial& operator=(const Serial&) = delete;

    Serial(Serial&&) = default;
    Serial& operator=(Serial&&) = default;
    
    void setBaudrate(speed_t baudrate);
    speed_t getBaudrate() const { return _baudrate; }
    
    void setStopBits(unsigned short stopBits);
    unsigned short getStopBits() const { return _stopBits; }

    void setBitSize(unsigned short bitSize);
    unsigned short getBitSize() const { return _bitSize; }
        
    void open();
    void write(const string &str);
    const string read();
    void close();
};

#endif /* Serial_hpp */

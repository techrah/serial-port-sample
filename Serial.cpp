//
//  Serial.cpp
//  testSerial
//
//  Created by Ryan Homer on 2021-07-24.
//

#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "Serial.hpp"
#include "Formatter.hpp"

using namespace std;

#define IS_OPEN(fd) ((fd) >= 0)

Serial::Serial(const string &port_,
               speed_t baudrate,
               bool parity_,
               unsigned short stopBits,
               unsigned short bitSize)
: port(port_), parity(parity_)
{
    setBaudrate(baudrate);
    setStopBits(stopBits);
    setBitSize(bitSize);
    open();
}

void Serial::setBaudrate(speed_t baudrate) {
    static const vector<int> baudrates = {
        B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
        , B7200, B14400, B28800, B57600, B76800, B115200, B230400
#endif
    };
    
    if (binary_search(baudrates.begin(), baudrates.end(), baudrate)) {
        _baudrate = baudrate;
        return;
    }
    
    fprintf(stderr, "Warning: %lu is not a valid baudrate. Using baudrate %lu.\n", baudrate, _baudrate);
}

void Serial::setBitSize(unsigned short bitSize) {
    if (bitSize >= 5 && bitSize <= 8) {
        _bitSize = bitSize;
    }
}

unsigned short Serial::mapBitSizeToMask(unsigned short bitSize) {
    switch (bitSize) {
        case 5: return CS5;
        case 6: return CS6;
        case 7: return CS7;
        case 8: return CS8;
    }
    throw invalid_argument("bitSize must be 5, 6, 7 or 8");
}

void Serial::setStopBits(unsigned short stopBits) {
    if (stopBits == 1 || stopBits == 2) {
        _stopBits = stopBits;
        return;
    }
    throw invalid_argument("stopBits must be 1 or 2");
}

void Serial::updateTty(struct termios& tty)
{
    // control modes
    updateTtyFlag(tty.c_cflag, PARENB, parity);
    updateTtyFlag(tty.c_cflag, CSTOPB, _stopBits == 2);
    clearTtyFlagBits(tty.c_cflag, CSIZE);
    setTtyFlagBits(tty.c_cflag, mapBitSizeToMask(_bitSize));
    clearTtyFlagBits(tty.c_cflag, CRTSCTS);
    setTtyFlagBits(tty.c_cflag, CREAD | CLOCAL);

    // local modes
    clearTtyFlagBits(tty.c_lflag, ICANON | ECHO | ECHOE | ECHONL | ISIG);
    
    // input modes
    clearTtyFlagBits(tty.c_iflag, IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    
    // output modes
    clearTtyFlagBits(tty.c_oflag, OPOST | ONLCR | OXTABS | ONOEOT);

    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;
}

void Serial::open() {
    if (IS_OPEN(_fd)) return;
    
    _fd = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (!IS_OPEN(_fd)) {
        cerr << strerror(errno) << endl;
        return;
    }
    
    struct termios tty;

    // get current tty settings
    if (tcgetattr(_fd, &tty) != 0) {
        throw runtime_error(Formatter("Error %i: %s\n", errno, strerror(errno)).msg());
    }

    updateTty(tty);

    // set baud rate
    cfsetispeed(&tty, _baudrate);
    cfsetospeed(&tty, _baudrate);

    // save tty settings
    if (tcsetattr(_fd, TCSANOW, &tty) != 0) {
        throw runtime_error(Formatter("Error %i: %s\n", errno, strerror(errno)).msg());
    }
}

void Serial::write(const string& str) {
    if (IS_OPEN(_fd)) {
        auto buf = str.c_str();
        ::write(_fd, buf, strlen(buf));
    } else {
        throw runtime_error("Error: Port is not open.");
    }
}

void Serial::close() {
    if (IS_OPEN(_fd)) {
        ::close(_fd);
    }
}

//
//  Formatter.hpp
//  testSerial
//
//  Created by Ryan Homer on 2021-07-24.
//

#ifndef Formatter_h
#define Formatter_h

static const unsigned short MSG_LEN = 256;

class Formatter {
private:
    char *_msg;
public:
    Formatter(const char *fmt, ...) __attribute__((format(printf, 2, 3))) : _msg(new char[MSG_LEN]) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(_msg, MSG_LEN, fmt, args);
        va_end(args);
    }
    ~Formatter() {
        delete[] _msg;
        _msg = nullptr;
    }
    const char* msg() const noexcept { return _msg; }
};

#endif /* Formatter_h */

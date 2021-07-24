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
    Formatter(const Formatter &f) : _msg(new char[MSG_LEN]) { strcpy(_msg, f._msg); }
    Formatter(Formatter &&f) : _msg(f._msg) { f._msg = nullptr; }
    Formatter& operator=(const Formatter &f) {
        // copy assign
        if (this == &f) return *this;
        delete[] _msg;
        _msg = new char[MSG_LEN];
        strcpy(_msg, f._msg);
        return *this;
    }
    Formatter& operator=(Formatter &&f) {
        // move assign
        if (this == &f) return *this;
        delete[] _msg;
        _msg = f._msg;
        f._msg = nullptr;
        return *this;
    }
    virtual ~Formatter() { delete[] _msg; _msg = nullptr; }
    const char* msg() const noexcept { return _msg; }
};

#endif /* Formatter_h */

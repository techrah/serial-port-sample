# serial-port-sample

This is sample C++20 code to demonstrate a Serial class that reads/writes to a serial port. It has only been tested on macOS so far so portability is not guaranteed.

## Write to Serial Port

```cpp
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "Serial.hpp"

int main() {
    try {
        // Configure settings ad-hoc
        Serial s;
        s.port = "/dev/cu.usbserial";
        s.setBaudrate(B115200);
        s.parity = false;
        s.setStopBits(1);
        s.setBitSize(8);

        // Must open before writing
        s.open();

        // Write something
        const string buf = "This is a test. This is a test. This is a test.\n";
        s.write(buf);

        // Wait a bit for the data to transfer before exiting
        sleep(2);

        s.close();
    } catch (exception &e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS
}
```

## Read from Serial Port

```cpp
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "Serial.hpp"

int main() {
    try {
        // Configure settings via constructor. No need to `open` explicitly.
        Serial s("/dev/cu.usbserial", //port
                 B115200,             // baudrate
                 false,               // parity
                 1,                   // stop bits
                 8);                  // bit size

        // Read
        while true {
            cout << s.read();
            sleep(1);
        }
    } catch (exception &e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
}
```

## C++xx Features

### C++20

#### Default initializer for bit-field

Serial.hpp

```cpp
// bit flags
uint8_t        parity:1 = false;
```

### C++11

#### noexcept specifier

Formatter.hpp

```cpp
const char* msg() const noexcept { return _msg; }
```

#### Non-aggregate type initialized with an initializer list

Serial.cpp

```cpp
static const vector<int> baudrates = {
    B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
    , B7200, B14400, B28800, B57600, B76800, B115200, B230400
#endif
};
```

#### Move semantics

Formatter.h

```cpp
// Move constructor
Formatter(Formatter &&f) : _msg(f._msg) { f._msg = nullptr; }

// move assignment
Formatter& operator=(Formatter &&f) {
    if (this == &f) return *this;
    delete[] _msg;
    _msg = f._msg;
    f._msg = nullptr;
    return *this;
}

```

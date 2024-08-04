//
// Created by Piotr on 09.09.2022.
//

#ifndef EMU_OS_SERIAL_H
#define EMU_OS_SERIAL_H

#include <string>
#include <vector>

#include "serial/parity.h"
#include "serial/stopbits.h"
#include "util/datatypes.h"

// todo: move these functions into a class. same with event/timer

namespace emu::os::serial {

#ifdef _WIN32
    using SerialPortHandle = void*;
#endif

    using emu::util::Milliseconds;

    struct SerialPortConfiguration {
        unsigned int baudRate;
        unsigned int byteSize;
        emu::serial::Parity parity;
        emu::serial::StopBits stopBits;
    };

    SerialPortHandle open(const std::string& portName);

    void close(SerialPortHandle handle);

    void configure(SerialPortHandle handle, const SerialPortConfiguration& config);

    void setNonBlockingRead(SerialPortHandle handle);

    void setReadIntervals(SerialPortHandle handle,
                          Milliseconds intervalTimeout,
                          Milliseconds timeoutMultiplier,
                          Milliseconds timeoutConstant);

    //void waitForRead(SerialPortHandle handle);

    unsigned int read(SerialPortHandle handle, uint8_t* buffer, std::size_t maxSize);

    unsigned int write(SerialPortHandle handle, const uint8_t* buffer, std::size_t maxSize);

} // os

#endif //EMU_OS_SERIAL_H

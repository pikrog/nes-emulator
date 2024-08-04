//
// Created by Piotr on 05.09.2022.
//

#include "serial/serialport.h"

namespace emu::serial {

    void SerialPort::receive(uint8_t byte) {
        data_ = byte;
        dataReady_ = true;

    }

    std::optional<uint8_t> SerialPort::read() {
        auto outputReady = dataReady_;
        dataReady_ = false;
        if(outputReady)
            return data_;
        return {};
    }

    void SerialPort::write(uint8_t byte) {
        transmit(byte);
    }

    SerialPort::SerialPort()
        : data_(),
        dataReady_(false) {

    }


} // serial
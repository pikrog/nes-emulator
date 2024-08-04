//
// Created by Piotr on 05.09.2022.
//

#include "serial/serialtransceiver.h"

namespace emu::serial {

    SerialTransceiver::SerialTransceiver()
        : remote_(nullptr) {

    }

    SerialTransceiver::~SerialTransceiver() {
        disconnect();
    }

    void SerialTransceiver::transmit(uint8_t byte) {
        if(remote_)
            remote_->receive(byte);
    }

    void SerialTransceiver::connect(SerialTransceiver &remote) {
        remote.remote_ = this;
        remote_ = &remote;
    }

    void SerialTransceiver::disconnect() {
        if(remote_)
            remote_->remote_ = nullptr;
        remote_ = nullptr;
    }


} // device
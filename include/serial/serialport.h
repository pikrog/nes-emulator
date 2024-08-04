//
// Created by Piotr on 05.09.2022.
//

#ifndef EMU_SERIALPORT_H
#define EMU_SERIALPORT_H

#include <optional>

#include "serialtransceiver.h"

namespace emu::serial {

    class SerialPort : public SerialTransceiver {
        uint8_t data_;
        bool dataReady_;

    public:
        SerialPort();

        [[nodiscard]] std::optional<uint8_t> read();
        void write(uint8_t byte);

    protected:
        void receive(uint8_t byte) override;
    };

} // serial

#endif //EMU_SERIALPORT_H

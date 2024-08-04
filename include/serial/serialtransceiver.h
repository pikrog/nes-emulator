//
// Created by Piotr on 05.09.2022.
//

#ifndef EMU_SERIALTRANSCEIVER_H
#define EMU_SERIALTRANSCEIVER_H

#include <cstdint>

namespace emu::serial {

    class SerialTransceiver {
        SerialTransceiver* remote_;

    public:
        SerialTransceiver();
        virtual ~SerialTransceiver();

        void connect(SerialTransceiver& remote);
        void disconnect();

    protected:
        void transmit(uint8_t byte);
        virtual void receive(uint8_t byte) {}
    };

}

#endif //EMU_SERIALTRANSCEIVER_H

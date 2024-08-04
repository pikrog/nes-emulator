//
// Created by Piotr on 05.09.2022.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef EMU_REGISTER_H
#define EMU_REGISTER_H

namespace emu::serial {

    enum class Register {
        DATA=0x0,
        STATUS,
        COMMAND,
        CONTROL
    };

    enum class StatusBit {
        PARITY_ERROR=0x1,
        FRAMING_ERROR=0x2,
        OVERRUN=0x4,
        RECEIVER_FULL=0x8,
        TRANSMITTER_EMPTY=0x10,
        DATA_CARRIER_DETECT=0x20,
        DATA_SET_READY=0x40,
        INTERRUPT=0x80
    };

    enum class CommandBit {
        DATA_TERMINAL_READY=0x1,
        RECEIVER_INTERRUPT_DISABLE=0x2,
        TRANSMITTER_CONTROL_0=0x4,
        TRANSMITTER_CONTROL_1=0x8,
        ECHO_MODE=0x10,
        PARITY_CHECK_CONTROL_0=0x20,
        PARITY_CHECK_CONTROL_1=0x40,
        PARITY_CHECK_CONTROL_2=0x80
    };

    enum class ControlBit {
        EXTERNAL_CLOCK_0=0x1,
        EXTERNAL_CLOCK_1=0x2,
        EXTERNAL_CLOCK_2=0x4,
        EXTERNAL_CLOCK_3=0x8,
        RECEIVER_CLOCK_SOURCE=0x10,
        WORD_LENGTH_0=0x20,
        WORLD_LENGTH_1=0x40,
        STOP_BITS=0x80,
    };

}

#endif //EMU_REGISTER_H

#pragma clang diagnostic pop
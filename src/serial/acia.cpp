//
// Created by Piotr on 05.09.2022.
//

#include "serial/acia.h"

namespace emu::serial {

    Acia::Acia()
        : receivedData_(0x0) {
        status_.set(StatusBit::TRANSMITTER_EMPTY);
        command_.set(CommandBit::RECEIVER_INTERRUPT_DISABLE);
    }

    uint8_t Acia::read(uint16_t address) {
        auto selectedRegister = static_cast<Register>(address & ADDRESS_MASK);
        switch(selectedRegister) {
            case Register::DATA:
                status_.clear(StatusBit::RECEIVER_FULL, StatusBit::OVERRUN,
                              StatusBit::PARITY_ERROR, StatusBit::FRAMING_ERROR);
                return receivedData_;
            case Register::STATUS:
                return *status_;
            case Register::COMMAND:
                return *command_;
            case Register::CONTROL:
                return *control_;

        }
        return 0x0;
    }

    void Acia::write(uint16_t address, uint8_t value) {
        auto selectedRegister = static_cast<Register>(address & ADDRESS_MASK);
        switch(selectedRegister) {
            case Register::DATA:
                transmit(value);
                if(command_.isSet(CommandBit::ECHO_MODE))
                    receive(value);
                break;
            case Register::STATUS:
                programReset();
                break;
            case Register::COMMAND:
                *command_ = value;
                break;
            case Register::CONTROL:
                *control_ = value;
                break;
        }
    }

    // todo: interrupts

    void Acia::receive(uint8_t byte) {
        if(status_.isSet(StatusBit::RECEIVER_FULL))
            status_.set(StatusBit::OVERRUN);
        status_.set(StatusBit::RECEIVER_FULL);
        receivedData_ = byte;
    }

    void Acia::programReset() {
        receivedData_ = 0x0;
        status_.clear(StatusBit::OVERRUN);
        command_.clear(
                CommandBit::RECEIVER_INTERRUPT_DISABLE,
                CommandBit::DATA_TERMINAL_READY,
                CommandBit::ECHO_MODE,
                CommandBit::TRANSMITTER_CONTROL_0,
                CommandBit::TRANSMITTER_CONTROL_1
                );
        command_.set(CommandBit::RECEIVER_INTERRUPT_DISABLE);
    }

} // device
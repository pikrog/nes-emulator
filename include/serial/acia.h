//
// Created by Piotr on 05.09.2022.
//

#ifndef EMU_ACIA_H
#define EMU_ACIA_H

#include <cstdint>

#include "aciaregisters.h"
#include "memory/interfacememory.h"
#include "serial/serialtransceiver.h"
#include "util/bitregister.h"

namespace emu::serial {

    class Acia final : public memory::InterfaceMemory<uint8_t, uint16_t>, public SerialTransceiver {
    public:
        static constexpr uint16_t ADDRESS_MASK = 0x3;

    private:
        template<typename T>
        using BitRegister = emu::util::bin::BitRegister<T, uint8_t>;

        uint8_t receivedData_;
        BitRegister<StatusBit> status_;
        BitRegister<CommandBit> command_;
        BitRegister<ControlBit> control_;

    public:
        Acia();

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;

    protected:
        void receive(uint8_t byte) override;

    private:
        void programReset();

    };

} // device

#endif //EMU_ACIA_H

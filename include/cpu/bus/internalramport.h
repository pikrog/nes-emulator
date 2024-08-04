//
// Created by Piotr on 25.09.2022.
//

#ifndef EMU_INTERNALRAMPORT_H
#define EMU_INTERNALRAMPORT_H

#include "bus/memoryport.h"
#include "memory/randomaccessmemory.h"

namespace emu::cpu {

    // todo: refactor this
    class InternalRamPort final : public bus::MemoryPort<memory::RandomAccessMemory<uint8_t, uint16_t>> {
    public:
        static constexpr uint16_t RAM_END_ADDRESS = 0x1fff;
        static constexpr uint16_t ADDRESS_MASK = 0x07ff;

        explicit InternalRamPort(memory::RandomAccessMemory<uint8_t, uint16_t>& memory)
            : MemoryPort(memory) {

        }

        uint8_t read(uint16_t address) override {
            return MemoryPort::read(address & ADDRESS_MASK);
        }

        void write(uint16_t address, uint8_t value) override {
            MemoryPort::write(address & ADDRESS_MASK, value);
        }

        [[nodiscard]] bool isActive(uint16_t address) const override {
            return address <= RAM_END_ADDRESS;
        }
    };

}

#endif //EMU_INTERNALRAMPORT_H

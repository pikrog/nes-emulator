//
// Created by Piotr on 27.09.2022.
//

#ifndef EMU_OAMDMAPORT_H
#define EMU_OAMDMAPORT_H

#include <cstdint>

#include "bus/interfaceport.h"

namespace emu::cpu {

    class Cpu;

    class OamDmaPort final : public bus::InterfacePort<uint8_t, uint16_t> {
        Cpu& cpu_;

    public:
        static constexpr uint16_t OAM_DMA_ADDRESS = 0x4014;
        static constexpr int TRANSFER_CYCLES = 513;

        explicit OamDmaPort(Cpu& cpu);

        [[nodiscard]] bool isActive(uint16_t address) const override;

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;
    };

} // cpu

#endif //EMU_OAMDMAPORT_H

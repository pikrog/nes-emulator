//
// Created by Piotr on 25.09.2022.
//

#ifndef EMU_PPUPORT_H
#define EMU_PPUPORT_H

#include "bus/memoryport.h"
#include "ppu/ppu.h"

namespace emu::cpu {

    class PpuPort final : public bus::MemoryPort<ppu::Ppu> {
    public:
        static constexpr uint16_t PPU_START_ADDRESS = 0x2000;
        static constexpr uint16_t PPU_END_ADDRESS = 0x3fff;

        explicit PpuPort(ppu::Ppu& ppu);

        [[nodiscard]] bool isActive(uint16_t address) const override;
    };

} // cpu

#endif //EMU_PPUPORT_H

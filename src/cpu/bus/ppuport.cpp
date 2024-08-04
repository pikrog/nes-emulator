//
// Created by Piotr on 25.09.2022.
//

#include "cpu/bus/ppuport.h"

namespace emu::cpu {


    PpuPort::PpuPort(ppu::Ppu &ppu) : MemoryPort(ppu) {

    }

    bool PpuPort::isActive(uint16_t address) const {
        return address >= PPU_START_ADDRESS && address <= PPU_END_ADDRESS;
    }

} // cpu
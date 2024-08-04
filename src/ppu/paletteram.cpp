//
// Created by Piotr on 19.09.2022.
//

#include "ppu/paletteram.h"

namespace emu::ppu {

    PaletteRam::PaletteRam()
        : RandomAccessMemory(PALETTE_SIZE) {

    }

    uint8_t PaletteRam::read(uint16_t address) {
        return RandomAccessMemory::read(map(address));
    }

    void PaletteRam::write(uint16_t address, uint8_t value) {
        RandomAccessMemory::write(map(address), value);
    }

    uint16_t PaletteRam::map(uint16_t address) {
        address &= ADDRESS_MASK;
        // backdrop mirroring
        if((address & 0x3) == 0)
            address &= ~0x10;
        return address;
    }

} // ppu
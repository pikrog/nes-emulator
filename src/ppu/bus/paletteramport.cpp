//
// Created by Piotr on 20.09.2022.
//

#include "ppu/bus/paletteramport.h"

namespace emu::ppu {

    PaletteRamPort::PaletteRamPort(PaletteRam &paletteRam)
        : MemoryPort(paletteRam) {

    }

    bool PaletteRamPort::isActive(uint16_t address) const {
        return (address & PALETTE_ADDRESS_MASK) == PALETTE_START_ADDRESS;
    }

    /*uint8_t PaletteRamPort::read(uint16_t address) {
        return MemoryPort::read(address & PaletteRam::PALETTE_SIZE);
    }

    void PaletteRamPort::write(uint16_t address, uint8_t value) {
        MemoryPort::write(address & PaletteRam::PALETTE_SIZE, value);
    };*/

} // ppu
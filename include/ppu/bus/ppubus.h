//
// Created by Piotr on 19.09.2022.
//

#ifndef EMU_PPUBUS_H
#define EMU_PPUBUS_H

#include <cstdint>

#include "bus/bus.h"

/*#include "bus/testbus.h"
#include "cartridgechrport.h"
#include "paletteramport.h"*/

namespace emu::ppu {

    //using PpuBus = bus::TestBus<uint8_t, uint16_t, CartridgeChrPort, PaletteRamPort>;
    using PpuBus = bus::Bus<uint8_t, uint16_t>;
    /*class PpuBus final : public bus::Bus<uint8_t, uint16_t> {
        PaletteRam paletteRam_;
        PaletteRamPort paletteRamPort_;

    public:
        PpuBus()
            : paletteRamPort_(paletteRam_) {
            attach(paletteRamPort_);
        }
    };*/

}

#endif //EMU_PPUBUS_H

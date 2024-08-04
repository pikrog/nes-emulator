//
// Created by Piotr on 18.09.2022.
//

#ifndef EMU_CARTRIDGEDATA_H
#define EMU_CARTRIDGEDATA_H

#include <vector>

#include <cstdint>

#include "mirroring.h"
#include "consoletype.h"
#include "timing.h"


namespace emu::cartridge {

    struct CartridgeData {
        std::vector<uint8_t> trainer, prgRom, chrRom;
        std::size_t prgRamSize, prgNvRamSize, chrRamSize, chrNvRamSize;
        bool hasBattery;
        Mirroring mirroring;
        int mapperId;
        int subMapperId;
        ConsoleType consoleType;
        Timing timing;
    };

}

#endif //EMU_CARTRIDGEDATA_H

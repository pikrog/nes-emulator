//
// Created by Piotr on 16.09.2022.
//

#ifndef EMU_NESHEADER_H
#define EMU_NESHEADER_H

#include <cstdint>

#include "mirroring.h"
#include "consoletype.h"
#include "timing.h"


namespace emu::cartridge {

    struct NesHeader {
        std::size_t prgRomSize;
        std::size_t chrRomSize;
        Mirroring mirroring;
        bool hasBattery;
        bool hasTrainer;
        int mapperId;
        int subMapperId;
        ConsoleType consoleType;
        std::size_t prgRamSize;
        std::size_t prgNvRamSize;
        std::size_t chrRamSize;
        std::size_t chrNvRamSize;
        Timing timing;
    };

}

#endif //EMU_NESHEADER_H

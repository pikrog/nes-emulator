//
// Created by Piotr on 15.09.2022.
//

#ifndef EMU_LOADER_H
#define EMU_LOADER_H

#include "cartridge.h"

namespace emu::cartridge {

    class Loader {
    public:
        static Cartridge open(const std::string& path);
    private:
        //static void readFlags6(Cartridge& cartridge, uint8_t flags);
        //static void readFlags7(Cartridge& cartridge, uint8_t flags);
        //static void readRemainingNes20Rom(Cartridge& cartridge)
    };

} // cartridge

#endif //EMU_LOADER_H

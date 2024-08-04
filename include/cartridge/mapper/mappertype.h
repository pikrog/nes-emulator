//
// Created by Piotr on 18.09.2022.
//

#ifndef EMU_MAPPERTYPE_H
#define EMU_MAPPERTYPE_H

#include <concepts>

#include <cstdint>

namespace emu::cartridge {

    class Cartridge;

    template<typename T>
    concept MapperType =
    requires(T t, uint16_t address, uint8_t value, Cartridge cartridge) {
        { T(cartridge) };
        { t.isActivePrg(address) } -> std::same_as<bool>;
        { t.readPrg(address) } -> std::same_as<uint8_t>;
        { t.writePrg(address, value) };
        { t.readChr(address) } -> std::same_as<uint8_t>;
        { t.writeChr(address, value) };
    };

}

#endif //EMU_MAPPERTYPE_H

//
// Created by Piotr on 19.09.2022.
//

#ifndef EMU_PALETTERAM_H
#define EMU_PALETTERAM_H

#include "memory/randomaccessmemory.h"

namespace emu::ppu {

    class PaletteRam final : public memory::RandomAccessMemory<uint8_t, uint16_t> {
    public:
        static constexpr std::size_t PALETTE_SIZE = 32;
        static constexpr uint16_t ADDRESS_MASK = 0x1f;

        PaletteRam();

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;

    private:
        [[nodiscard]] static uint16_t map(uint16_t address);
    };

} // ppu

#endif //EMU_PALETTERAM_H

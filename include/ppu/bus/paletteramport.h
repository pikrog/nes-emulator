//
// Created by Piotr on 20.09.2022.
//

#ifndef EMU_PALETTERAMPORT_H
#define EMU_PALETTERAMPORT_H

#include "ppu/paletteram.h"
#include "bus/memoryport.h"

namespace emu::ppu {

    class PaletteRamPort final : public bus::MemoryPort<PaletteRam> {
    public:
        static constexpr uint16_t PALETTE_START_ADDRESS = 0x3f00;
        static constexpr uint16_t PALETTE_ADDRESS_MASK = 0xff00;

        explicit PaletteRamPort(PaletteRam& paletteRam);

        [[nodiscard]] bool isActive(uint16_t address) const override;
        /*[[nodiscard]] uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;*/

    };

} // ppu

#endif //EMU_PALETTERAMPORT_H

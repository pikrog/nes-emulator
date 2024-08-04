//
// Created by Piotr on 19.09.2022.
//

#ifndef EMU_CARTRIDGECHRPORT_H
#define EMU_CARTRIDGECHRPORT_H

#include <cstdint>

#include "cartridge/cartridge.h"
#include "bus/memoryport.h"

namespace emu::ppu {

    class CartridgeChrPort final : public bus::InterfacePort<uint8_t, uint16_t> {
        using Cartridge = cartridge::Cartridge;

        Cartridge& cartridge_;

    public:
        static constexpr uint16_t CHR_END_ADDRESS = 0x3eff;

        explicit CartridgeChrPort(Cartridge& cartridge);

        [[nodiscard]] bool isActive(uint16_t address) const override;

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;

    };

} // cartridge

#endif //EMU_CARTRIDGECHRPORT_H

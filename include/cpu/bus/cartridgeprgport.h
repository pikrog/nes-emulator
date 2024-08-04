//
// Created by Piotr on 19.09.2022.
//

#ifndef EMU_CARTRIDGEPRGPORT_H
#define EMU_CARTRIDGEPRGPORT_H

#include <cstdint>

#include "cartridge/cartridge.h"
#include "bus/memoryport.h"

namespace emu::cpu {

    class CartridgePrgPort final : public bus::InterfacePort<uint8_t, uint16_t> {
        using Cartridge = cartridge::Cartridge;

        Cartridge& cartridge_;

    public:
        explicit CartridgePrgPort(Cartridge& cartridge);

        [[nodiscard]] bool isActive(uint16_t address) const override;
        //[[nodiscard]] uint8_t getActivePins(uint16_t address) const override;

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;

    };

} // cartridge

#endif //EMU_CARTRIDGEPRGPORT_H

//
// Created by Piotr on 19.09.2022.
//

#include "ppu/bus/cartridgechrport.h"

namespace emu::ppu {

    CartridgeChrPort::CartridgeChrPort(Cartridge &cartridge)
        : InterfacePort(),
          cartridge_(cartridge) {

    }

    uint8_t CartridgeChrPort::read(uint16_t address) {
        return cartridge_.readChr(address);
    }

    void CartridgeChrPort::write(uint16_t address, uint8_t value) {
        cartridge_.writeChr(address, value);
    }

    bool CartridgeChrPort::isActive(uint16_t address) const {
        return address <= CHR_END_ADDRESS;
    }

} // cartridge
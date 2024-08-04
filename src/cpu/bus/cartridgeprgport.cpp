//
// Created by Piotr on 19.09.2022.
//

#include "cpu/bus/cartridgeprgport.h"

namespace emu::cpu {

    CartridgePrgPort::CartridgePrgPort(cartridge::Cartridge &cartridge)
        : cartridge_(cartridge),
        InterfacePort() {

    }

    /*uint8_t CartridgePrgPort::getActivePins(uint16_t address) const {
        return cartridge_.getActivePinsPrg(address);
    }*/

    bool CartridgePrgPort::isActive(uint16_t address) const {
        return cartridge_.isActive(address);
    }

    uint8_t CartridgePrgPort::read(uint16_t address) {
        return cartridge_.readPrg(address);
    }

    void CartridgePrgPort::write(uint16_t address, uint8_t value) {
        cartridge_.writePrg(address, value);
    }


} // cartridge
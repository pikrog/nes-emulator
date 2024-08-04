//
// Created by Piotr on 05.09.2022.
//

#include "cpu/bus/aciaport.h"

namespace emu::cpu {

    AciaPort::AciaPort(serial::Acia& serial)
        : MemoryPort(serial) {
                     //[] (uint16_t address) { return (address & ACIA_ADDRESS_MASK) == ACIA_START_ADDRESS; },
                     //[] (uint16_t address)-> uint16_t { return address & ~ACIA_ADDRESS_MASK; }) {

    }

    bool AciaPort::isActive(uint16_t address) const {
        return (address & ACIA_ADDRESS_MASK) == ACIA_START_ADDRESS;
    }

    /*uint8_t AciaPort::getActivePins(uint16_t address) const {
        return ((address & ACIA_ADDRESS_MASK) == ACIA_START_ADDRESS) ? 0xff : 0x0;
    }*/

    /*uint8_t AciaPort::read(uint16_t address) {
        return MemoryPort::read(map(address));
    }

    void AciaPort::write(uint16_t address, uint8_t value) {
        MemoryPort::write(map(address), value);
    }

    uint16_t AciaPort::map(uint16_t address) {
        return address & ~ACIA_ADDRESS_MASK;
    }*/

} // bus
//
// Created by Piotr on 30.09.2022.
//

#include "ppu/bus/controllerport.h"

namespace emu::cpu {

    ControllerPort::ControllerPort(Controller& controller, ControllerPort::Player player)
        : InterfacePort(0x1),
          controller_(controller) {
        switch(player) {
            case Player::FIRST:
                readAddress_ = CONTROLLER_ADDRESS;
                break;
            case Player::SECOND:
                readAddress_ = CONTROLLER_ADDRESS + 1;
                break;
        }
    }

    bool ControllerPort::isActive(uint16_t address) const {
        return address == CONTROLLER_ADDRESS || address == readAddress_;
    }

    uint8_t ControllerPort::read(uint16_t address) {
        return controller_.readNextBit();
    }

    void ControllerPort::write(uint16_t address, uint8_t value) {
        controller_.updateStrobe(value);
    }

} // cpu
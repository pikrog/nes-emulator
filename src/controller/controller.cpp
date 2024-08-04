//
// Created by Piotr on 30.09.2022.
//

#include "controller/controller.h"

namespace emu::controller {

    Controller::Controller()
        : isStrobeSet_(false),
        probedButtonStates_() {

    }

    void Controller::press(Button button) {
        buttonStates_.set(button);
    }

    void Controller::release(Button button) {
        buttonStates_.clear(button);
    }

    void Controller::clock() {
        strobe();
    }

    void Controller::updateStrobe(bool set) {
        isStrobeSet_ = set;
        strobe();
    }

    uint8_t Controller::readNextBit() {
        auto bit = probedButtonStates_ & 0x1;
        probedButtonStates_ >>= 1;
        return bit;
    }

    void Controller::strobe() {
        if(isStrobeSet_)
            probedButtonStates_ = *buttonStates_;
    }


} // Controller
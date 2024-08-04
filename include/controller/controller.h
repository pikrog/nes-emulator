//
// Created by Piotr on 30.09.2022.
//

#ifndef EMU_CONTROLLER_H
#define EMU_CONTROLLER_H

#include "button.h"

#include "util/bitregister.h"

namespace emu::controller {

    class Controller {
        util::bin::BitRegister<Button, uint8_t> buttonStates_;
        unsigned int probedButtonStates_;

        bool isStrobeSet_;

    public:
        Controller();

        void press(Button button);
        void release(Button button);

        void clock();

        void updateStrobe(bool set);
        uint8_t readNextBit();

    private:
        void strobe();

    };

} // Controller

#endif //EMU_CONTROLLER_H

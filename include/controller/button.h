//
// Created by Piotr on 30.09.2022.
//

#ifndef EMU_BUTTON_H
#define EMU_BUTTON_H

namespace emu::controller {

    enum class Button {
        A=0x1,
        B=0x2,
        SELECT=0x4,
        START=0x8,
        UP=0x10,
        DOWN=0x20,
        LEFT=0x40,
        RIGHT=0x80
    };

}

#endif //EMU_BUTTON_H

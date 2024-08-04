//
// Created by Piotr on 31.08.2022.
//

#ifndef EMU_FLAG_H
#define EMU_FLAG_H

namespace emu::cpu {

    enum class Flag {
        CARRY = 0x1,
        ZERO = 0x2,
        INTERRUPT_DISABLE = 0x4,
        DECIMAL = 0x8,
        //B_LOW = 0x10,
        //B_HIGH = 0x20,
        OVERFLOW = 0x40,
        NEGATIVE = 0x80,
    };

}

#endif //EMU_FLAG_H

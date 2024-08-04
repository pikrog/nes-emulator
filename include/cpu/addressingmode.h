//
// Created by Piotr on 01.09.2022.
//

#ifndef EMU_ADDRESSINGMODE_H
#define EMU_ADDRESSINGMODE_H

namespace emu::cpu {

    enum class AddressingMode {
        UNSPECIFIED,
        IMPLICIT,
        IMMEDIATE,
        ACCUMULATOR,
        ZERO_PAGE,
        ZERO_PAGE_X,
        ZERO_PAGE_Y,
        ABSOLUTE,
        ABSOLUTE_X,
        ABSOLUTE_Y,
        INDIRECT,
        INDIRECT_X,
        INDIRECT_Y,
        RELATIVE,
    };

}

#endif //EMU_ADDRESSINGMODE_H

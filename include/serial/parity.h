//
// Created by Piotr on 06.09.2022.
//

#ifndef EMU_PARITY_H
#define EMU_PARITY_H

namespace emu::serial {

    enum class Parity {
        NO_PARITY=0,
        ODD_PARITY,
        EVEN_PARITY,
        MARK_PARITY,
        SPACE_PARITY
    };

}

#endif //EMU_PARITY_H

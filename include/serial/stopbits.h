//
// Created by Piotr on 06.09.2022.
//

#ifndef EMU_STOPBITS_H
#define EMU_STOPBITS_H

namespace emu::serial {

    enum class StopBits {
        ONE_STOP_BIT=0x0,
        ONE_AND_HALF_STOP_BITS,
        TWO_STOP_BITS,
    };

}

#endif //EMU_STOPBITS_H

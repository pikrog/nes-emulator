//
// Created by Piotr on 15.09.2022.
//

#ifndef EMU_TIMING_H
#define EMU_TIMING_H

namespace emu::cartridge {

    enum class Timing {
        NTSC,
        PAL,
        MULTIPLE_REGION,
        DENDY
    };

}

#endif //EMU_TIMING_H

//
// Created by Piotr on 01.09.2022.
//

#ifndef EMU_CPUBUS_H
#define EMU_CPUBUS_H

#include <cstdint>

#include "bus/bus.h"

/*#include "bus/testbus.h"
#include "internalramport.h"
#include "cartridgeprgport.h"
#include "ppuport.h"
#include "oamdmaport.h"*/

namespace emu::cpu {

    // todo bus::Bus should be a generic template
    using CpuBus = emu::bus::Bus<uint8_t, uint16_t>;
    //using CpuBus = emu::bus::TestBus<uint8_t, uint16_t, InternalRamPort, PpuPort, CartridgePrgPort, OamDmaPort>;

} // bus

#endif //EMU_CPUBUS_H

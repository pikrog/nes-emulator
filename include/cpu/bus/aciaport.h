//
// Created by Piotr on 05.09.2022.
//

#ifndef EMU_ACIAPORT_H
#define EMU_ACIAPORT_H


#include <cstdint>

#include "bus/interfaceport.h"
#include "bus/memoryport.h"
#include "serial/acia.h"


namespace emu::cpu {

    class AciaPort final : public bus::MemoryPort<serial::Acia> {
    public:
        static constexpr uint16_t ACIA_START_ADDRESS = 0x7f70;
        static constexpr uint16_t ACIA_ADDRESS_MASK = 0xfffc;

        explicit AciaPort(serial::Acia& serial);

        [[nodiscard]] bool isActive(uint16_t address) const override;

    };

} // bus`

#endif //EMU_ACIAPORT_H

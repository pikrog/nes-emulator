//
// Created by Piotr on 18.09.2022.
//

#ifndef EMU_MAPPER0_H
#define EMU_MAPPER0_H

#include <span>

#include <cstdint>

namespace emu::cartridge {

    class Cartridge;

    class Mapper0 {
    public:
        static constexpr uint16_t PRG_START_ADDRESS = 0x6000;
        static constexpr uint16_t NO_PRG_RAM_PRG_START_ADDRESS = 0x8000;

    private:
        bool isVerticalMirroring_;

        struct MemorySelector {
            std::span<uint8_t> memory;
            uint16_t mask;
        };

        std::array<MemorySelector, 2> prgSelect_;
        std::array<MemorySelector, 2> chrSelect_;

        //uint8_t prgBusData_;
        uint16_t prgStartAddress_;
    public:
        explicit Mapper0(Cartridge& cartridge);

        [[nodiscard]] bool isActivePrg(uint16_t address) const;
        uint8_t readPrg(uint16_t address);
        void writePrg(uint16_t address, uint8_t value);

        uint8_t readChr(uint16_t address);
        void writeChr(uint16_t address, uint8_t value);
    private:
        [[nodiscard]] uint8_t* getPrgLocation(uint16_t address) const;
        [[nodiscard]] uint8_t* getChrLocation(uint16_t address) const;
    };

} // cartridge

#endif //EMU_MAPPER0_H

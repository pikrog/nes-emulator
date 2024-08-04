//
// Created by Piotr on 15.09.2022.
//

#ifndef NES_CARTRIDGE_H
#define NES_CARTRIDGE_H

#include <vector>
#include <string>
#include <span>
#include <variant>

#include <cstdint>

#include "cartridgedata.h"
#include "mirroring.h"
#include "consoletype.h"
#include "timing.h"
#include "mapper/mappertype.h"
#include "mapper/mappers.h"
#include "bus/interfaceport.h"

namespace emu::cartridge {

    class Loader;

    class Cartridge {
    public:
        static constexpr std::size_t INTERNAL_VRAM_SIZE = 2048;

    private:
        template<MapperType... Mappers>
        using MapperVariant = std::variant<Mappers...>;

        using Mapper = MapperVariant<Mapper0>; // ... , Mapper1, Mapper2, ...

        // should vram be here or on the bus?
        std::vector<uint8_t> prgRom_, chrRom_, trainer_, prgRam_, prgNvRam_, chrRam_, chrNvRam_, vRam_;

        bool hasBattery_;
        Mirroring mirroring_;
        int mapperId_;
        int subMapperId_;
        ConsoleType consoleType_;
        Timing timing_;

        Mapper mapper_;

    public:
        Cartridge();
        explicit Cartridge(CartridgeData&& data);

        [[nodiscard]] bool hasBattery() const;
        [[nodiscard]] Mirroring getMirroring() const;
        [[nodiscard]] int getMapperId() const;
        [[nodiscard]] int getSubMapperId() const;
        [[nodiscard]] ConsoleType getConsoleType() const;
        [[nodiscard]] Timing getTiming() const;

        [[nodiscard]] std::span<uint8_t> getPrgRom();
        [[nodiscard]] std::span<uint8_t> getPrgRam();
        [[nodiscard]] std::span<uint8_t> getPrgNvRam();

        [[nodiscard]] std::span<uint8_t> getChrRom();
        [[nodiscard]] std::span<uint8_t> getChrRam();
        [[nodiscard]] std::span<uint8_t> getChrNvRam();

        [[nodiscard]] std::span<uint8_t> getTrainer();

        [[nodiscard]] std::span<uint8_t> getVRam();

        [[nodiscard]] bool isActive(uint16_t address) const;
        uint8_t readPrg(uint16_t address);
        void writePrg(uint16_t address, uint8_t value);

        uint8_t readChr(uint16_t address);
        void writeChr(uint16_t address, uint8_t value);

    private:
        Mapper createMapper();

    };

} // cartridge

#endif //NES_CARTRIDGE_H

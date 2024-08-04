//
// Created by Piotr on 16.09.2022.
//

#ifndef EMU_NESHEADERREADER_H
#define EMU_NESHEADERREADER_H

#include <span>

#include <cstdint>

#include "nesheader.h"
#include "version.h"

namespace emu::cartridge {

    class NesHeaderReader {
    public:
        static constexpr std::size_t HEADER_SIZE = 16;

    private:
        static constexpr uint8_t FILE_IDENTIFIER[] = "NES\x1a";
        static constexpr std::size_t LEGACY_PADDING_OFFSET = 12;
        static constexpr uint8_t LEGACY_PADDING[] = { 0x0, 0x0, 0x0, 0x0};
        static constexpr std::size_t FILE_IDENTIFIER_SIZE = sizeof(FILE_IDENTIFIER) - 1;

        enum Offset {
            PRG_ROM_SIZE_LSB = 4,
            CHR_ROM_SIZE_LSB,
            FLAGS_6,
            FLAGS_7,
            MAPPER_MSB_SUBMAPPER,
            PRG_CHR_ROM_SIZE_MSB,
            PRG_RAM_SIZE,
            CHR_RAM_SIZE,
            TIMING,
            EXTENDED_CONSOLE_TYPE,
            MISCELLANEOUS_ROMS,
            DEFAULT_EXPANSION_DEVICE,

            // legacy
            LEGACY_PRG_RAM_SIZE = FLAGS_7 + 1,
            LEGACY_TV_SYSTEM,
        };

        std::span<const uint8_t> header_;

    public:
        explicit NesHeaderReader(std::span<const uint8_t> header, bool doStrictLegacyCheck = false);

        [[nodiscard]] NesHeader read() const;

        [[nodiscard]] bool isValidFormat() const;
        [[nodiscard]] Version readVersion(bool doStrictLegacyCheck) const;
        [[nodiscard]] std::size_t readPrgRomSize() const;
        [[nodiscard]] std::size_t readChrRomSize() const;
        [[nodiscard]] Mirroring readMirroring() const;
        [[nodiscard]] bool readHasBattery() const;
        [[nodiscard]] bool readHasTrainer() const;
        [[nodiscard]] int readMapperId() const;
        [[nodiscard]] int readSubMapperId() const;
        [[nodiscard]] ConsoleType readConsoleType() const;
        [[nodiscard]] std::size_t readPrgRamSize() const;
        [[nodiscard]] std::size_t readPrgNvRamSize() const;
        [[nodiscard]] std::size_t readChrRamSize() const;
        [[nodiscard]] std::size_t readChrNvRamSize() const;
        [[nodiscard]] Timing readTiming() const;

        [[nodiscard]] int legacyReadMapperId() const;
        [[nodiscard]] std::size_t legacyReadPrgRamSize() const;
        [[nodiscard]] Timing legacyReadTiming() const;

    private:
        [[nodiscard]] uint8_t readByte(Offset offset) const;
        [[nodiscard]] NesHeader readLegacy() const;
        [[nodiscard]] NesHeader readNew() const;
    };

} // cartridge

#endif //EMU_NESHEADERREADER_H

//
// Created by Piotr on 16.09.2022.
//

#include <stdexcept>

#include <cstring>

#include "cartridge/exception.h"
#include "cartridge/nesheaderreader.h"
#include "util/bin.h"

using emu::util::bin::evaluateExponentMultiplierSize;
using emu::util::bin::evaluateShifted64Size;
using emu::util::bin::makeWord;
using emu::util::bin::getLowerNibble;
using emu::util::bin::getHigherNibble;


namespace emu::cartridge {

    NesHeaderReader::NesHeaderReader(std::span<const uint8_t> header, bool doStrictLegacyCheck)
        : header_(header) {
        if(header.size() < HEADER_SIZE)
            throw std::invalid_argument("iNES header has " + std::to_string(HEADER_SIZE) + " bytes");
        if(!isValidFormat())
            throw InvalidFormat();
        if(readVersion(doStrictLegacyCheck) == Version::UNKNOWN)
            throw UnsupportedError("unknown iNES format");
    }

    NesHeader NesHeaderReader::read() const {
        if(readVersion(false) == Version::LEGACY)
            return readLegacy();
        return readNew();
    }

    bool NesHeaderReader::isValidFormat() const {
        return std::memcmp(&header_[0], FILE_IDENTIFIER, FILE_IDENTIFIER_SIZE) == 0;
    }

    Version NesHeaderReader::readVersion(bool doStrictLegacyCheck) const {
        auto version = (readByte(Offset::FLAGS_7) >> 2) & 0x3;
        if(version == 0x2)
            return Version::NEW;
        else if(version == 0x0
            && (!doStrictLegacyCheck || std::memcmp(&header_[LEGACY_PADDING_OFFSET],
                                                    LEGACY_PADDING,
                                                    sizeof(LEGACY_PADDING)) == 0))
            return Version::LEGACY;
        return Version::UNKNOWN;
    }

    std::size_t NesHeaderReader::readPrgRomSize() const {
        auto sizeLsb = readByte(Offset::PRG_ROM_SIZE_LSB);
        auto sizeMsb = getLowerNibble(readByte(Offset::PRG_CHR_ROM_SIZE_MSB));
        if(sizeMsb <= 0xe)
            return makeWord(sizeLsb, sizeMsb) * 16 * 1024; // 16 KiB units
        return evaluateExponentMultiplierSize(sizeLsb);
    }

    std::size_t NesHeaderReader::readChrRomSize() const {
        auto sizeLsb = readByte(Offset::CHR_ROM_SIZE_LSB);
        auto sizeMsb = getHigherNibble(readByte(Offset::PRG_CHR_ROM_SIZE_MSB));
        if(sizeMsb <= 0xe)
            return makeWord(sizeLsb, sizeMsb) * 8 * 1024; // 8 KiB units
        return evaluateExponentMultiplierSize(sizeLsb);
    }

    Mirroring NesHeaderReader::readMirroring() const {
        auto byte = readByte(Offset::FLAGS_6);
        Mirroring mirroring = (byte & 0x1) ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;
        if(byte & 0x8)
            mirroring = Mirroring::OTHER;
        return mirroring;
    }

    bool NesHeaderReader::readHasBattery() const {
        return readByte(Offset::FLAGS_6) & 0x2;
    }

    bool NesHeaderReader::readHasTrainer() const {
        return readByte(Offset::FLAGS_6) & 0x4;
    }

    int NesHeaderReader::readMapperId() const {
        auto lowerNibble = getHigherNibble(readByte(Offset::FLAGS_6));
        auto middleNibble = getHigherNibble(readByte(Offset::FLAGS_7));
        auto higherNibble = getLowerNibble(readByte(Offset::MAPPER_MSB_SUBMAPPER));
        return lowerNibble | (middleNibble << 4) | (higherNibble << 8);
    }

    int NesHeaderReader::readSubMapperId() const {
        return getHigherNibble(readByte(Offset::MAPPER_MSB_SUBMAPPER));
    }

    ConsoleType NesHeaderReader::readConsoleType() const {
        auto byte = readByte(Offset::FLAGS_7);
        switch(byte & 0x3) {
            case 0x0:
                return ConsoleType::NINTENDO_ENTERTAINMENT_SYSTEM;
            case 0x1:
                return ConsoleType::NINTENDO_VS_SYSTEM;
            case 0x2:
                return ConsoleType::NINTENDO_PLAYCHOICE_10;
            case 0x3:
            default:
                return ConsoleType::EXTENDED_CONSOLE_TYPE;
        }
    }

    std::size_t NesHeaderReader::readPrgRamSize() const {
        auto byte = readByte(Offset::PRG_RAM_SIZE);
        auto shift = getLowerNibble(byte);
        return evaluateShifted64Size(shift);
    }

    std::size_t NesHeaderReader::readPrgNvRamSize() const {
        auto byte = readByte(Offset::PRG_RAM_SIZE);
        auto shift = getHigherNibble(byte);
        return evaluateShifted64Size(shift);
    }

    std::size_t NesHeaderReader::readChrRamSize() const {
        auto byte = readByte(Offset::CHR_RAM_SIZE);
        auto shift = getLowerNibble(byte);
        return evaluateShifted64Size(shift);
    }

    std::size_t NesHeaderReader::readChrNvRamSize() const {
        auto byte = readByte(Offset::CHR_RAM_SIZE);
        auto shift = getHigherNibble(byte);
        return evaluateShifted64Size(shift);
    }

    Timing NesHeaderReader::readTiming() const {
        auto byte = readByte(Offset::TIMING);
        switch(byte & 0x3) {
            case 0x0:
                return Timing::NTSC;
            case 0x1:
                return Timing::PAL;
            case 0x2:
                return Timing::MULTIPLE_REGION;
            case 0x3:
            default:
                return Timing::DENDY;
        }
    }

    int NesHeaderReader::legacyReadMapperId() const {
        auto lowerNibble = getHigherNibble(readByte(Offset::FLAGS_6));
        auto higherNibble = getHigherNibble(readByte(Offset::FLAGS_7));
        return lowerNibble | (higherNibble << 4);
    }

    std::size_t NesHeaderReader::legacyReadPrgRamSize() const {
        auto byte = readByte(Offset::LEGACY_PRG_RAM_SIZE);
        byte = std::max<uint8_t>(byte, 1);
        return byte * 8 * 1024;
    }

    Timing NesHeaderReader::legacyReadTiming() const {
        auto byte = readByte(Offset::LEGACY_TV_SYSTEM);
        if(byte & 0x1)
            return Timing::PAL;
        return Timing::NTSC;
    }

    uint8_t NesHeaderReader::readByte(Offset offset) const {
        return header_[static_cast<std::size_t>(offset)];
    }

    NesHeader NesHeaderReader::readLegacy() const {
        return {
                .prgRomSize = readPrgRomSize(),
                .chrRomSize = readChrRomSize(),
                .mirroring = readMirroring(),
                .hasBattery = readHasBattery(),
                .hasTrainer = readHasTrainer(),
                .mapperId = legacyReadMapperId(),
                .prgRamSize = legacyReadPrgRamSize(),
                .timing = legacyReadTiming(),
        };
    }

    NesHeader NesHeaderReader::readNew() const {
        return {
                .prgRomSize = readPrgRomSize(),
                .chrRomSize = readChrRomSize(),
                .mirroring = readMirroring(),
                .hasBattery = readHasBattery(),
                .hasTrainer = readHasTrainer(),
                .mapperId = readMapperId(),
                .subMapperId = readSubMapperId(),
                .consoleType = readConsoleType(),
                .prgRamSize = readPrgRamSize(),
                .prgNvRamSize = readPrgNvRamSize(),
                .chrRamSize = readChrRamSize(),
                .chrNvRamSize = readChrNvRamSize(),
                .timing = readTiming(),
        };
    }

} // cartridge
//
// Created by Piotr on 15.09.2022.
//

#include "cartridge/loader.h"

#include <sstream>
#include <string>

#include <cstring>

#include "cartridge/nesheaderreader.h"
#include "util/misc.h"
#include "cartridge/cartridgedata.h"

using emu::util::misc::openBinaryFile;
using emu::util::misc::readBytes;


namespace emu::cartridge {

    namespace {

        const std::size_t TRAINER_SIZE = 512;

    }

    Cartridge Loader::open(const std::string& path) {
        auto file = openBinaryFile(path);
        file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        auto headerBuffer = readBytes(file, NesHeaderReader::HEADER_SIZE);
        auto header = NesHeaderReader(headerBuffer).read();
        return Cartridge({
                .trainer = header.hasTrainer ? readBytes(file, TRAINER_SIZE) : std::vector<uint8_t>(),
                .prgRom = readBytes(file, header.prgRomSize),
                .chrRom = readBytes(file, header.chrRomSize),
                .prgRamSize = header.prgRamSize,
                .prgNvRamSize = header.prgNvRamSize,
                .chrRamSize = header.chrRamSize,
                .chrNvRamSize = header.chrNvRamSize,
                .hasBattery = header.hasBattery,
                .mirroring = header.mirroring,
                .mapperId = header.mapperId,
                .subMapperId = header.subMapperId,
                .consoleType = header.consoleType,
                .timing = header.timing
        });
    }


} // cartridge
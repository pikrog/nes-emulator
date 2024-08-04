//
// Created by Piotr on 04.09.2022.
//

#ifndef EMU_MISC_H
#define EMU_MISC_H

#include <fstream>
#include <string>
#include <vector>

#include <cstdint>

namespace emu::util::misc {

    [[nodiscard]] std::ifstream openBinaryFile(const std::string& path);

    [[nodiscard]] std::vector<uint8_t> readBinaryFile(const std::string& path);

    [[nodiscard]] std::vector<uint8_t> readBytes(std::istream& stream, std::size_t n);

    struct Mos6502Executable {
        std::vector<uint8_t> program;
        uint16_t offset;
    };

    [[nodiscard]] Mos6502Executable readMos6502Executable(const std::string& path);

    [[nodiscard]] uint8_t petscii2ascii(uint8_t chr);

}

#endif //EMU_MISC_H

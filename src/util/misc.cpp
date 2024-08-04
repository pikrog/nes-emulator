//
// Created by Piotr on 04.09.2022.
//

#include "util/misc.h"

#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iterator>

#include "util/bin.h"

namespace emu::util::misc {

    std::ifstream openBinaryFile(const std::string& path) {
        std::ifstream file;
        file.unsetf(std::ifstream::skipws);
        file.open(&path[0], std::ifstream::binary);
        if (!file)
            throw std::ifstream::failure("failed to open the file");
        return file;
    }

    std::vector<uint8_t> readBinaryFile(const std::string& path) {
        auto file = openBinaryFile(path);
        file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        auto fileSize = std::filesystem::file_size(path);
        return readBytes(file, fileSize);
        /*::vector<uint8_t> buffer;
        buffer.reserve(fileSize);
        auto fileIterator = std::istream_iterator<uint8_t>(file);
        auto inserter = std::back_inserter(buffer);
        std::copy_n(fileIterator, fileSize, inserter);
        return buffer;*/
    }

    std::vector<uint8_t> readBytes(std::istream& stream, std::size_t n) {
        std::vector<uint8_t> buffer;
        buffer.reserve(n);
        auto iterator = std::istream_iterator<uint8_t>(stream);
        auto inserter = std::back_inserter(buffer);
        std::copy_n(iterator, n, inserter);
        //stream.read(reinterpret_cast<char*>(&buffer[0]), n);
        return buffer;
    }

    Mos6502Executable readMos6502Executable(const std::string& path) {
        auto file = openBinaryFile(path);
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        auto fileSize = std::filesystem::file_size(path);
        std::vector<uint8_t> buffer;
        uint8_t offsetLsb, offsetMsb;
        file >> offsetLsb >> offsetMsb;
        auto offset = emu::util::bin::makeWord(offsetLsb, offsetMsb);
        buffer.reserve(fileSize + offset);
        buffer.resize(offset);
        auto fileIterator = std::istream_iterator<uint8_t>(file);
        auto inserter = std::back_inserter(buffer);
        std::copy_n(fileIterator, fileSize - 2, inserter);
        return {buffer, offset};
    }

    /* https://thec64community.online/post/438/thread */
    uint8_t petscii2ascii(uint8_t chr) {
        if(chr >= 97 && chr <= 122)
            chr -= 32;
        else if(chr >= 65 && chr <= 90)
            chr += 32;
        else if(chr >= 192 && chr <= 223) {
            chr -= 96;
            if(chr >= 97 && chr <= 122) {
                chr -= 32;
            }
        }
        return chr;
    }
}
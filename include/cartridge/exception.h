//
// Created by Piotr on 15.09.2022.
//

#ifndef EMU_EXCEPTION_H
#define EMU_EXCEPTION_H

#include <stdexcept>

namespace emu::cartridge {

    class InvalidFormat : public std::runtime_error {
    public:
        InvalidFormat() : std::runtime_error("the file is not an NES ROM") {}
    };

    class UnsupportedError : public std::runtime_error {
    public:
        explicit UnsupportedError(
                const std::string& what = "the ROM requires features not supported by this emulator"
                        ) : std::runtime_error(what) {}
    };
}

#endif //EMU_EXCEPTION_H

//
// Created by Piotr on 02.09.2022.
//

#ifndef EMU_BIN_H
#define EMU_BIN_H

#include <concepts>

#include <cstdint>

namespace emu::util::bin {

    [[nodiscard]] constexpr uint8_t getLowerByte(uint16_t word) {
        return word & 0xff;
    }

    [[nodiscard]] constexpr uint8_t getHigherByte(uint16_t word) {
        return (word >> 8) & 0xff;
    }

    [[nodiscard]] constexpr uint8_t getLowerNibble(uint8_t byte) {
        return byte & 0xf;
    }

    [[nodiscard]] constexpr uint8_t getHigherNibble(uint8_t byte) {
        return byte >> 4;
    }

    [[nodiscard]] constexpr uint16_t makeWord(uint8_t lowerByte, uint8_t higherByte) {
        return lowerByte | (higherByte << 8);
    }

    [[nodiscard]] constexpr bool isPageCrossed(uint16_t address1, uint16_t address2) {
        return (address1 & 0xff00) != (address2 & 0xff00);
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr bool isNegativeU2(T value) {
        return (value >> (sizeof(T) * 8 - 1)) & 1;
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr T negateU2(T value) {
        return ~value + 1;
    }

    template<std::unsigned_integral T, std::signed_integral U=int>
    [[nodiscard]] constexpr U decodeU2SignedInt(T value) {
        return isNegativeU2(value) ? negateU2(value) * -1 : value;
    }

    /* http://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64BitsDiv */
    [[nodiscard]] constexpr uint8_t reverse(uint8_t byte) {
        return (byte * 0x0202020202ULL & 0x010884422010ULL) % 1023;
    }

    /*template<std::unsigned_integral T>
    [[nodiscard]] constexpr T reverse(T bits) {

    }*/

    [[nodiscard]] constexpr std::size_t evaluateExponentMultiplierSize(uint8_t byte) {
        auto exponent = (byte & 0xfc) >> 2;
        auto multiplier = byte & 0x3;
        return (2ull << exponent) * (multiplier * 2 + 1);
    }

    [[nodiscard]] constexpr std::size_t evaluateShifted64Size(uint8_t shift) {
        return shift ? 64 << shift : 0;
    }

}

#endif //EMU_BIN_H

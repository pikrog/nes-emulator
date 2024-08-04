//
// Created by Piotr on 08.09.2022.
//

#ifndef EMU_BITREGISTER_H
#define EMU_BITREGISTER_H

#include <cstdint>
#include <concepts>

#include "util/math.h"


namespace emu::util::bin {

    template<typename BitType, std::unsigned_integral T = uint8_t>
    class BitRegister {
        T register_;

    public:
        explicit BitRegister(T initialValue = T()) : register_(initialValue) {}

        template<std::same_as<BitType> ...Args>
        void set(Args... args) {
            auto bits = (... | static_cast<T>(args));
            register_ |= bits;
        }

        template<std::same_as<BitType> ...Args>
        void clear(Args... args) {
            auto bits = (... | static_cast<T>(args));
            register_ &= ~bits;
        }

        void update(bool set, BitType bit) {
            auto value = static_cast<T>(bit);
            if(set)
                register_ |= value;
            else
                register_ &= ~value;
        }

        [[nodiscard]] bool isSet(BitType bit) const {
            return register_ & static_cast<T>(bit);
        }

        [[nodiscard]] bool isClear(BitType bit) const {
            return !isSet(bit);
        }

        [[nodiscard]] explicit operator T&() {
            return register_;
        }

        [[nodiscard]] explicit operator T() const {
            return register_;
        }

        [[nodiscard]] T& operator*() {
            return register_;
        }

        [[nodiscard]] const T& operator*() const {
            return register_;
        }
    };

    template<typename T>
    using BitRegister8 = BitRegister<T, uint8_t>;

}

#endif //EMU_BITREGISTER_H

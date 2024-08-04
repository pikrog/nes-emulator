//
// Created by Piotr on 08.09.2022.
//

#ifndef EMU_MATH_H
#define EMU_MATH_H

#include <concepts>
#include <stdexcept>

namespace emu::util::math {

    template<std::integral T = int>
    [[nodiscard]] constexpr T naturalPower(T base, T exponent) {
        if(exponent < 0)
            throw std::invalid_argument("exponent was negative");
        return exponent == 0 ? 1 : base * naturalPower<T>(base, exponent - 1);
    }

}

#endif //EMU_MATH_H

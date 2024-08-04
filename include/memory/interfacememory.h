//
// Created by Piotr on 04.09.2022.
//

#ifndef EMU_INTERFACEMEMORY_H
#define EMU_INTERFACEMEMORY_H

#include <concepts>

namespace emu::memory {

    template<typename T, typename U = std::size_t>
    class InterfaceMemory {
    public:
        using Value = T;
        using Address = U;

        virtual ~InterfaceMemory() = default;

        virtual T read(U address) = 0;
        virtual void write(U address, T value) = 0;
    };

}

#endif //EMU_INTERFACEMEMORY_H

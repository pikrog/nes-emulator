//
// Created by Piotr on 05.09.2022.
//

#ifndef EMU_INTERFACEPORT_H
#define EMU_INTERFACEPORT_H

#include <limits>

#include <cstddef>

namespace emu::bus {

    template<typename T, typename U = std::size_t>
    class InterfacePort {
        T activePins_;

    public:
        explicit InterfacePort(T activePins = std::numeric_limits<T>::max()) : activePins_(activePins) {}
        virtual ~InterfacePort() = default;

        [[nodiscard]] T getActivePins() const { return activePins_; };
        [[nodiscard]] virtual bool isActive(U address) const = 0;

        virtual T read(U address)  = 0;
        virtual void write(U address, T value) = 0;
    };

} // bus

#endif //EMU_INTERFACEPORT_H

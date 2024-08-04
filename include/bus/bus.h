//
// Created by Piotr on 01.09.2022.
//

#ifndef EMU_BUS_H
#define EMU_BUS_H

#include <vector>

#include "bus/interfaceport.h"

namespace emu::bus {

    template<typename T, typename U>
    class Bus {
    public:
        using InterfacePort = InterfacePort<T, U>;

    private:
        std::vector<InterfacePort*> ports_;
        T data_;

    public:
        Bus();

        virtual ~Bus() = default;

        virtual T read(U address);
        virtual void write(U address, T value);

        void attach(InterfacePort& port);

    private:
        //[[nodiscard]] InterfacePort* findDesignatedPort(U address) const;

    };

} // bus

#endif //EMU_BUS_H

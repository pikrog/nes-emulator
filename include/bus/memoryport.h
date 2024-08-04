//
// Created by Piotr on 05.09.2022.
//

#ifndef EMU_MEMORYPORT_H
#define EMU_MEMORYPORT_H

#include <concepts>
#include <functional>
#include <limits>

#include "bus/interfaceport.h"
#include "memory/interfacememory.h"

namespace emu::bus {

    // todo: this class might be redundant
    // todo: make typenames more verbose just like it is here:
    template<typename T, typename Value = typename T::Value, typename Address = typename T::Address>
    requires std::derived_from<T, memory::InterfaceMemory<Value,Address>>
    class MemoryPort : public InterfacePort<Value, Address> {
        T& memory_;

    public:
        explicit MemoryPort(T& memory, Value activePins = std::numeric_limits<Value>::max())
            : InterfacePort<Value, Address>(activePins),
            memory_(memory) {

        }

        Value read(Address address) override {
            return memory_.read(address);
        }

        void write(Address address, Value value) override {
            memory_.write(address, value);
        }

    };

} // bus

#endif //EMU_MEMORYPORT_H

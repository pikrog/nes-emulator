//
// Created by Piotr on 26.09.2022.
//

#ifndef EMU_TESTBUS_H
#define EMU_TESTBUS_H

#include <tuple>
#include <concepts>

#include "interfaceport.h"

namespace emu::bus {

    template<typename Value, typename Address, std::derived_from<InterfacePort<Value, Address>>... Args>
    class TestBus {
        std::tuple<Args*...> ports_;
        Value data_;

    public:
        TestBus() : data_(0x0) {}

        template<std::derived_from<InterfacePort<Value, Address>> T>
        void attach(T& port) {
            std::get<T*>(ports_) = &port;
        }

        Value read(Address address) {
            std::apply([&](auto... args) {
                (([&](auto port) {
                    if (port->isActive(address)) {
                        auto activePins = port->getActivePins();
                        auto value = port->read(address);
                        data_ = (data_ & ~activePins) | (value & activePins);
                    }
                })(args), ...);
            }, ports_);
            return data_;
        }

        void write(Address address, Value value) {
            std::apply([&](auto... args) {
                (([&](auto port) {
                    if (port->isActive(address)) {
                        port->write(address, value);
                    }
                })(args), ...);
            }, ports_);
        }

    };

}

#endif //EMU_TESTBUS_H

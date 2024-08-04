//
// Created by Piotr on 01.09.2022.
//

#include "bus/bus.h"

namespace emu::bus {

    template<typename T, typename U>
    Bus<T, U>::Bus()
            : data_() {

    }

    template<typename T, typename U>
    T Bus<T, U>::read(U address) {
        for(auto port : ports_) {
            if(port->isActive(address)) {
                auto activePins = port->getActivePins();
                auto value = port->read(address);
                // setup data and emulate open bus
                data_ = (data_ & ~activePins) | (value & activePins);
            }
        }
        return data_;
    }

    template<typename T, typename U>
    void Bus<T, U>::write(U address, T value) {
        data_ = value;
        for(auto port : ports_) {
            if(port->isActive(address))
                port->write(address, value);
        }
    }

    template<typename T, typename U>
    void Bus<T, U>::attach(InterfacePort &port) {
        ports_.push_back(&port);
    }

} // bus
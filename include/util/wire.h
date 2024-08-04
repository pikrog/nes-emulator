//
// Created by Piotr on 21.09.2022.
//

#ifndef EMU_WIRE_H
#define EMU_WIRE_H

namespace emu::util {

    template<typename T>
    class Wire {
        T& object_;
        using Signal = void (T::*)();
        Signal signal_;
    public:
        explicit Wire(T& object, Signal signal) : object_(object), signal_(signal) {}
        Wire(const Wire&) = default;
        Wire& operator=(const Wire&) = default;

        void signal() {
            (object_.*signal_)();
        }
    };

}

#endif //EMU_WIRE_H

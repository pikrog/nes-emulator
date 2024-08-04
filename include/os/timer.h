//
// Created by Piotr on 09.09.2022.
//

#ifndef EMU_OS_TIMER_H
#define EMU_OS_TIMER_H

#include <chrono>

#include "os/event.h"
#include "util/datatypes.h"

namespace emu::os::timer {

    using emu::util::Milliseconds;
    using emu::os::event::Event;

    class Timer {
#ifdef _WIN32
        unsigned int handle_;
        Event event_;
#endif

public:
        explicit Timer(Milliseconds period, Milliseconds resolution = Milliseconds(1));
        ~Timer();

        void wait();
    };

} // timer

#endif //EMU_OS_TIMER_H
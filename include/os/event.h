//
// Created by Piotr on 09.09.2022.
//

#ifndef EMU_OS_EVENT_H
#define EMU_OS_EVENT_H

namespace emu::os::timer {
    class Timer;
}

namespace emu::os::event {

    class Event {
#ifdef _WIN32
        friend class emu::os::timer::Timer;
        void* handle_;
#endif

    public:
        Event();
        ~Event();

        void wait();
        void set();
    };

}

#endif //EMU_OS_EVENT_H

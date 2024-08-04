//
// Created by Piotr on 09.09.2022.
//

#include "os/timer.h"

#include "os/error.h"

using emu::os::getLastSystemError;

#ifdef _WIN32
#include <windows.h>

namespace emu::os::timer {

    Timer::Timer(Milliseconds period, Milliseconds resolution) {
        handle_ = timeSetEvent(
                period.count(),
                resolution.count(),
                (LPTIMECALLBACK)event_.handle_,
                0,
                TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
        if(!handle_)
            throw getLastSystemError();
    }

    Timer::~Timer() {
        timeKillEvent(handle_);
    }

    void Timer::wait() {
        event_.wait();
    }

}
#endif
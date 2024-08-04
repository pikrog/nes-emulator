//
// Created by Piotr on 09.09.2022.
//

#include "os/event.h"

#include "os/error.h"

#ifdef _WIN32
#include <windows.h>

namespace emu::os::event {

    Event::Event() {
        handle_ = CreateEvent(nullptr, false, false, nullptr);
        if(!handle_)
            throw getLastSystemError();
    }

    Event::~Event() {
        CloseHandle(handle_);
    }

    void Event::wait() {
        WaitForSingleObject(handle_, INFINITE);
    }

    void Event::set() {
        SetEvent(handle_);
    }


}
#endif
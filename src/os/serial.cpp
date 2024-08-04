//
// Created by Piotr on 09.09.2022.
//

#include "os/serial.h"

#include "os/error.h"

using emu::os::getLastSystemError;

#ifdef _WIN32
#include <windows.h>

namespace emu::os::serial {

    SerialPortHandle open(const std::string& portName) {
        auto handle = CreateFile(
                portName.c_str(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                nullptr,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
                nullptr);
        if (handle == INVALID_HANDLE_VALUE)
            throw getLastSystemError();
        return handle;
    }

    void close(SerialPortHandle handle) {
        CancelIoEx(handle, nullptr);
        CloseHandle(handle);
    }

    void configure(SerialPortHandle handle, const SerialPortConfiguration& config) {
        DCB dcb;
        if(!GetCommState(handle, &dcb))
            throw getLastSystemError();
        dcb.BaudRate = config.baudRate;
        dcb.ByteSize = config.byteSize;
        dcb.Parity = static_cast<uint8_t>(config.parity);
        dcb.StopBits = static_cast<uint8_t>(config.stopBits);
        if(!SetCommState(handle, &dcb))
            throw getLastSystemError();
    }

    static void updateReadTimeouts(SerialPortHandle handle,
                                   unsigned int interval,
                                   unsigned int multiplier,
                                   unsigned int constant) {
        COMMTIMEOUTS timeouts;
        if(!GetCommTimeouts(handle, &timeouts))
            throw getLastSystemError();
        timeouts.ReadIntervalTimeout = interval;
        timeouts.ReadTotalTimeoutConstant = constant;
        timeouts.ReadTotalTimeoutMultiplier = multiplier;
        if(!SetCommTimeouts(handle, &timeouts))
            throw getLastSystemError();
    }

    void setNonBlockingRead(SerialPortHandle handle) {
        updateReadTimeouts(handle, MAXDWORD, 0, 0);
    }

    void setReadIntervals(SerialPortHandle handle,
                          Milliseconds intervalTimeout,
                          Milliseconds timeoutMultiplier,
                          Milliseconds timeoutConstant) {
        updateReadTimeouts(handle, intervalTimeout.count(), timeoutMultiplier.count(), timeoutConstant.count());
    }

    // RAII wrapper for the "unmanaged" resource
    class AsyncIoEvent {
        HANDLE event_;
    public:
        AsyncIoEvent()
            : event_(CreateEvent(nullptr, true, false, nullptr)) {
            if(!event_)
                throw getLastSystemError();
        }

        ~AsyncIoEvent() {
            CloseHandle(event_);
        }

        explicit operator HANDLE() {
            return event_;
        }
    };

    unsigned int read(SerialPortHandle handle, uint8_t* buffer, std::size_t size) {
        DWORD bytesRead;
        AsyncIoEvent event;
        OVERLAPPED overlapped = {
                .hEvent = HANDLE(event)
        };
        if(!ReadFile(handle, buffer, size, &bytesRead, &overlapped)) {
            if(GetLastError() != ERROR_IO_PENDING)
                throw getLastSystemError();
            if(WaitForSingleObject(overlapped.hEvent, INFINITE))
                throw getLastSystemError();
            if(!GetOverlappedResult(handle, &overlapped, &bytesRead, false))
                throw getLastSystemError();
        }
        return bytesRead;
    }

    unsigned int write(SerialPortHandle handle, const uint8_t* buffer, std::size_t size) {
        DWORD bytesWritten;
        AsyncIoEvent event;
        OVERLAPPED overlapped = {
                .hEvent = HANDLE(event)
        };
        if(!WriteFile(handle, buffer, size, &bytesWritten, &overlapped)) {
            if(GetLastError() != ERROR_IO_PENDING)
                throw getLastSystemError();
            if(WaitForSingleObject(overlapped.hEvent, INFINITE))
                throw getLastSystemError();
            if(!GetOverlappedResult(handle, &overlapped, &bytesWritten, false))
                throw getLastSystemError();
        }
        return bytesWritten;
    }

} // os

#endif
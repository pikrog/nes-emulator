//
// Created by Piotr on 07.09.2022.
//

#include "os/error.h"

#ifdef _WIN32
#include <windows.h>

namespace emu::os {

    /* https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code */
    std::string getLastErrorString() {
        LPSTR message;
        auto errorId = GetLastError();
        FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                errorId,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &message,
                0,
                nullptr
        );
        std::string errorString(message);
        LocalFree(message);
        return errorString;
    }

    std::error_code getLastErrorCode() {
        return {static_cast<int>(GetLastError()), std::system_category()}; // windows error values don't exceed 16000
    }

    std::system_error getLastSystemError() {
        return {getLastErrorCode()};
    }

}

#endif
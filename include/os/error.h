//
// Created by Piotr on 07.09.2022.
//

#ifndef EMU_OS_ERROR_H
#define EMU_OS_ERROR_H

#include <string>
#include <system_error>

#include "serial/parity.h"
#include "serial/stopbits.h"

namespace emu::os {

    [[nodiscard]] std::string getLastErrorString();

    [[nodiscard]] std::error_code getLastErrorCode();

    [[nodiscard]] std::system_error getLastSystemError();

}

#endif //EMU_OS_ERROR_H

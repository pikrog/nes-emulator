//
// Created by Piotr on 10.09.2022.
//

#ifndef EMU_DATATYPES_H
#define EMU_DATATYPES_H

#include <chrono>

namespace emu::util {

    using Milliseconds = std::chrono::milliseconds;

    using Signal = void (*)();

}

#endif //EMU_DATATYPES_H

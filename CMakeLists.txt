cmake_minimum_required(VERSION 3.23)
project(emu)

set(CMAKE_CXX_STANDARD 20)

include_directories("${PROJECT_SOURCE_DIR}/include")
link_directories("${PROJECT_SOURCE_DIR}/lib")

add_executable(emu src/main.cpp src/cpu/cpu.cpp include/cpu/cpu.h src/memory/randomaccessmemory.cpp include/memory/randomaccessmemory.h include/cpu/flag.h include/cpu/opcode.h include/cpu/addressingmode.h src/bus/bus.cpp include/bus/bus.h include/cpu/bus/cpubus.h include/util/bin.h include/memory/interfacememory.h include/util/misc.h src/util/misc.cpp include/bus/interfaceport.h include/bus/memoryport.h src/bus/bus.impl.cpp src/memory/randomaccessmemory.impl.cpp src/cpu/bus/aciaport.cpp include/cpu/bus/aciaport.h src/serial/serialtransceiver.cpp include/serial/acia.h include/serial/aciaregisters.h src/serial/acia.cpp include/serial/serialtransceiver.h src/serial/serialport.cpp include/serial/serialport.h src/serial/hostserialport.cpp include/serial/hostserialport.h include/serial/parity.h include/serial/stopbits.h include/os/error.h src/os/error.cpp src/util/bin.cpp include/util/bitregister.h include/util/math.h src/os/serial.cpp include/os/serial.h src/os/timer.cpp include/os/timer.h include/os/event.h src/os/event.cpp include/util/datatypes.h src/cartridge/cartridge.cpp include/cartridge/cartridge.h include/cartridge/mirroring.h include/cartridge/consoletype.h include/cartridge/exception.h src/cartridge/loader.cpp include/cartridge/loader.h include/cartridge/timing.h include/cartridge/version.h src/cartridge/nesheaderreader.cpp include/cartridge/nesheaderreader.h include/cartridge/nesheader.h include/cartridge/mapper/mappers.h src/cartridge/mapper/mapper0.cpp include/cartridge/mapper/mapper0.h include/cartridge/mapper/mappertype.h include/cartridge/cartridgedata.h src/cpu/bus/cartridgeprgport.cpp include/cpu/bus/cartridgeprgport.h src/ppu/bus/cartridgechrport.cpp include/ppu/bus/cartridgechrport.h src/ppu/ppu.cpp include/ppu/ppu.h include/ppu/bus/ppubus.h src/ppu/paletteram.cpp include/ppu/paletteram.h src/ppu/bus/paletteramport.cpp include/ppu/bus/paletteramport.h include/ppu/ppuregisters.h include/util/wire.h include/cpu/bus/internalramport.h src/cpu/bus/ppuport.cpp include/cpu/bus/ppuport.h include/bus/testbus.h src/cpu/bus/oamdmaport.cpp include/cpu/bus/oamdmaport.h src/controller/controller.cpp include/controller/controller.h include/controller/button.h src/cpu/bus/controllerport.cpp include/ppu/bus/controllerport.h)

target_link_libraries(emu mingw32 SDL2main SDL2)

if(WIN32)
    target_link_libraries(emu winmm)
endif()

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(DEBUG)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
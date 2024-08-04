//
// Created by Piotr on 27.09.2022.
//

#include "cpu/bus/oamdmaport.h"

#include "cpu/cpu.h"
#include "cpu/bus/ppuport.h"

namespace emu::cpu {


    OamDmaPort::OamDmaPort(Cpu &cpu)
        : InterfacePort(0x0),
          cpu_(cpu) {

    }

    bool OamDmaPort::isActive(uint16_t address) const {
        return address == OAM_DMA_ADDRESS;
    }

    uint8_t OamDmaPort::read(uint16_t address) {
        return 0x0;
    }

    void OamDmaPort::write(uint16_t address, uint8_t value) {
        // takes TRANSFER_CYCLES to complete. add +1 cycle if occured on odd cycle
        cpu_.remainingCycles_ += TRANSFER_CYCLES + (cpu_.elapsedCycles_ % 2);
        for(unsigned int i = 0; i <= 0xff; i++) {
            uint16_t sourceAddress = (value << 8) | i;
            auto byte = cpu_.bus_.read(sourceAddress);
            cpu_.bus_.write(PpuPort::PPU_START_ADDRESS + static_cast<uint16_t>(ppu::Register::OAM_DATA), byte);
        }
    }

} // cpu
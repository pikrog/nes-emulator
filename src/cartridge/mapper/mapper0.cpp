//
// Created by Piotr on 18.09.2022.
//

#include "cartridge/mapper/mapper0.h"

#include "cartridge/cartridge.h"

namespace emu::cartridge {

    // todo: potentially unsafe memory accesses. resize vectors or use safe accessors (::at)
    Mapper0::Mapper0(Cartridge& cartridge)
        : isVerticalMirroring_(cartridge.getMirroring() == Mirroring::VERTICAL),
          prgStartAddress_(PRG_START_ADDRESS) {
        auto prgRam = cartridge.getPrgRam();

        // PRG RAM at 0x6000 - 0x7fff
        prgSelect_[0].memory = prgRam;
        if(prgRam.empty()) {
            prgStartAddress_ = NO_PRG_RAM_PRG_START_ADDRESS;
//            prgSelect_[0].memory = std::span<uint8_t>(&prgBusData_, 1); // open bus
//            prgSelect_[0].mask = 0x0;
        } else if(prgRam.size() <= 2 * 1024) {
            // 2 KiB PRG RAM + 3 * 2 KiB Mirrors
            prgSelect_[0].mask = 0x7ff;
        } else {
            // 4 KiB PRG RAM + 4 KiB Mirror
            prgSelect_[0].mask = 0xfff;
        }

        // PRG ROM at 0x8000 - 0xffff
        auto prgRom = cartridge.getPrgRom();
        prgSelect_[1].memory = prgRom;
        if(prgRom.size() <= 16 * 1024)
            // 16 KiB PRG ROM + 16 KiB Mirror
            prgSelect_[1].mask = 0x3FFF;
        else
            // 32 KiB PRG ROM
            prgSelect_[1].mask = 0x7FFF;

        auto chrRom = cartridge.getChrRom();
        chrSelect_[0].memory = chrRom;
        // 8 KiB Cartridge CHR ROM
        chrSelect_[0].mask = 0x1fff;

        auto vRam = cartridge.getVRam();
        chrSelect_[1].memory = vRam;
        // 2 KiB Console Video RAM
        chrSelect_[1].mask = 0x3ff;
    }

    bool Mapper0::isActivePrg(uint16_t address) const {
        return address >= prgStartAddress_;
    }

    uint8_t Mapper0::readPrg(uint16_t address) {
        return *getPrgLocation(address);
    }

    void Mapper0::writePrg(uint16_t address, uint8_t value) {
        *getPrgLocation(address) = value;
    }

    uint8_t Mapper0::readChr(uint16_t address) {
        return *getChrLocation(address);
    }

    void Mapper0::writeChr(uint16_t address, uint8_t value) {
        *getChrLocation(address) = value;
    }

    uint8_t* Mapper0::getPrgLocation(uint16_t address) const {
        auto& selectedMemory = prgSelect_[address >> 15];
        return &selectedMemory.memory[address & selectedMemory.mask];
    }

    uint8_t* Mapper0::getChrLocation(uint16_t address) const {
        auto addressBit13 = (address >> 13) & 0x1;
        auto& selectedMemory = chrSelect_[addressBit13];
        auto translatedAddress = (address & selectedMemory.mask)
                                 // A13 is wired to VRAM CS. Evaluated CIRAM A10 bit will be applied only when A13 = 1
                                 | (addressBit13 * (
                                    // Vertical Mirroring: Connect PPU A10 to CIRAM A10
                                    (isVerticalMirroring_ * (address & 0x400))
                                    // Horizontal Mirroring: Connect PPU A11 to CIRAM A10
                                    | (!isVerticalMirroring_ * ((address & 0x800) >> 1))
                                    )
                                 );
        return &selectedMemory.memory[translatedAddress];
    }

} // cartridge
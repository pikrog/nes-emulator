//
// Created by Piotr on 15.09.2022.
//

#include "cartridge/cartridge.h"
#include "cartridge/exception.h"


namespace emu::cartridge {

    Cartridge::Cartridge()
        : hasBattery_(),
          mirroring_(),
          mapperId_(),
          subMapperId_(),
          consoleType_(),
          timing_(),
          mapper_(Mapper0(*this)),
          vRam_(INTERNAL_VRAM_SIZE) {

    }

    Cartridge::Cartridge(CartridgeData&& data)
        : trainer_(std::move(data.trainer)),
          prgRom_(std::move(data.prgRom)),
          chrRom_(std::move(data.chrRom)),
          prgRam_(std::vector<uint8_t>(data.prgRamSize)),
          prgNvRam_(std::vector<uint8_t>(data.prgNvRamSize)),
          chrRam_(std::vector<uint8_t>(data.chrRamSize)),
          chrNvRam_(std::vector<uint8_t>(data.chrNvRamSize)),
          vRam_(INTERNAL_VRAM_SIZE),
          hasBattery_(data.hasBattery),
          mirroring_(data.mirroring),
          mapperId_(data.mapperId),
          subMapperId_(data.subMapperId),
          consoleType_(data.consoleType),
          timing_(data.timing),
          mapper_(createMapper()) {

    }

    bool Cartridge::isActive(uint16_t address) const {
        return std::visit([&] (auto& object) { return object.isActivePrg(address); }, mapper_);
    }

    uint8_t Cartridge::readPrg(uint16_t address) {
        return std::visit([&] (auto& object) { return object.readPrg(address); }, mapper_);
    }

    void Cartridge::writePrg(uint16_t address, uint8_t value) {
        std::visit([&] (auto& object) { object.writePrg(address, value); }, mapper_);
    }

    uint8_t Cartridge::readChr(uint16_t address) {
        return std::visit([&] (auto& object) { return object.readChr(address); }, mapper_);
    }

    void Cartridge::writeChr(uint16_t address, uint8_t value) {
        std::visit([&] (auto& object) { object.writeChr(address, value); }, mapper_);
    }

    bool Cartridge::hasBattery() const {
        return hasBattery_;
    }

    Mirroring Cartridge::getMirroring() const {
        return mirroring_;
    }

    int Cartridge::getMapperId() const {
        return mapperId_;
    }

    int Cartridge::getSubMapperId() const {
        return subMapperId_;
    }

    ConsoleType Cartridge::getConsoleType() const {
        return consoleType_;
    }

    Timing Cartridge::getTiming() const {
        return timing_;
    }

    std::span<uint8_t> Cartridge::getPrgRom() {
        return prgRom_;
    }

    std::span<uint8_t> Cartridge::getPrgRam() {
        return prgRam_;
    }

    std::span<uint8_t> Cartridge::getPrgNvRam() {
        return prgNvRam_;
    }

    std::span<uint8_t> Cartridge::getChrRom() {
        return chrRom_;
    }

    std::span<uint8_t> Cartridge::getChrRam() {
        return chrRam_;
    }

    std::span<uint8_t> Cartridge::getChrNvRam() {
        return chrNvRam_;
    }

    std::span<uint8_t> Cartridge::getTrainer() {
        return trainer_;
    }

    std::span<uint8_t> Cartridge::getVRam() {
        return vRam_;
    }

    Cartridge::Mapper Cartridge::createMapper() {
        switch(mapperId_) {
            case 0:
                return Mapper0(*this);
            default:
                throw UnsupportedError("mapper " + std::to_string(mapperId_) + " is not supported");
        }
    }


} // cartridge
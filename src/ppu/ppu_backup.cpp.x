//
// Created by Piotr on 19.09.2022.
//

#include "ppu/ppu.h"

namespace emu::ppu {

    Ppu::Ppu(PpuBus &bus, Wire verticalBlankInterrupt, std::span<uint8_t> screen)
        : scanLine_(PRERENDER_SCANLINE),
          scanLineCycle_(0),
          isVerticalBlanking_(false),
          elapsedFrames_(0),
          tempTileIndex_(),
          tempAttributes_(),
          tempLowerPattern_(),
          tempUpperPattern_(),
          backgroundAttributes_(),
          backgroundLowerPattern_(),
          backgroundUpperPattern_(),
          oamAddress_(0x0),
          spriteSlots_(0),
          evaluatedSprites_(0),
          isSecondWrite_(false),
          fineScrollX_(0x0),
          fineScrollY_(0x0),
          tempVRamAddress_(0x0),
          currentVRamAddress_(0x0),
          vRamData_(0x0),
          cpuDataBus_(0x0),
          oamRam_(),
          secondaryOamRam_(),
          sprites_(),
          bus_(bus),
          verticalBlankInterrupt_(verticalBlankInterrupt),
          screenIndex_(0),
          screenOffset_(0),
          screen_(screen) {
        if(screen_.size() < SCREEN_WIDTH * SCREEN_HEIGHT)
            throw std::invalid_argument("the screen buffer is too small");
    }

    void Ppu::clock() {
        if(isRendering()) {
            // fetch next tile data
            if((scanLineCycle_ - WORK_BEGIN_CYCLE) % 8 == 0) {
                backgroundLowerPattern_ = backgroundLowerQueue_ >> 8;
                backgroundUpperPattern_ = backgroundUpperQueue_ >> 8;
                backgroundAttributes_ = backgroundAttributesQueue_ >> 8;
            }

            // render single pixel
            if(scanLine_ != PRERENDER_SCANLINE
                && scanLineCycle_ >= WORK_BEGIN_CYCLE && scanLineCycle_ <= RENDERING_END_CYCLE) {
                auto patternLsb = (backgroundLowerPattern_ & 0x80) >> 7;
                auto patternMsb = (backgroundUpperPattern_ & 0x80) >> 7;
                auto attributes = backgroundAttributes_;
                outputPixel(patternLsb, patternMsb, attributes);
                backgroundLowerPattern_ <<= 1;
                backgroundUpperPattern_ <<= 1;
            }

            // handle tile fetches
            if((scanLineCycle_ - WORK_BEGIN_CYCLE) % 8 == 0) {
                if (scanLineCycle_ >= SPRITE_FETCH_BEGIN_CYCLE
                    && scanLineCycle_ <= SPRITE_FETCH_END_CYCLE) {

                } else {
                    if(scanLineCycle_ == REDUNDANT_NT_FETCH_BEGIN_CYCLE) {
                        bus_.read(evaluateBackgroundNametableAddress());
                        bus_.read(evaluateBackgroundAttributeAddress());
                    } else {
                        auto index = bus_.read(evaluateBackgroundNametableAddress());
                        backgroundAttributesQueue_ <<= 8;
                        backgroundAttributesQueue_ |= bus_.read(evaluateBackgroundAttributeAddress());
                        backgroundLowerQueue_ <<= 8;
                        backgroundLowerQueue_ |= bus_.read(evaluateBackgroundPatternAddress(index, 0));
                        backgroundUpperQueue_ <<= 8;
                        backgroundUpperQueue_ |= bus_.read(evaluateBackgroundPatternAddress(index, 1));
                        incrementCoarseX();
                    }
                }
            }
            /*if(scanLineCycle_ % 2 == 1) {
                auto phase = static_cast<FetchPhase>((scanLineCycle_ - 1) / 2 % 4);
                if (scanLineCycle_ >= REDUNDANT_NT_FETCH_BEGIN_CYCLE)
                    phase = FetchPhase::NAMETABLE;

                if (scanLineCycle_ >= SPRITE_FETCH_BEGIN_CYCLE
                    && scanLineCycle_ <= SPRITE_FETCH_END_CYCLE) {
                    // fetchSpriteData();
                } else {
                    auto isDataReady = fetchTileData(phase);
                    if (isDataReady) {
                        backgroundAttributesQueue_ <<= 8;
                        backgroundAttributesQueue_ |= tempAttributes_;
                        backgroundLowerQueue_ <<= 8;
                        backgroundLowerQueue_ |= tempLowerPattern_ << fineScrollX_;
                        backgroundUpperQueue_ <<= 8;
                        backgroundUpperQueue_ |= tempUpperPattern_ << fineScrollX_;
                        incrementCoarseX();
                    }
                }
            }*/

            // modify current VRAM address accordingly
            if (scanLineCycle_ == VERTICAL_INCREMENT_CYCLE) {
                incrementFineScrollY();
            } else if (scanLineCycle_ == HORIZONTAL_RESTORE_CYCLE) {
                restoreHorizontalPosition();
            } else if (scanLine_ == PRERENDER_SCANLINE && scanLineCycle_ == VERTICAL_RESTORE_BEGIN_CYCLE) {
                restoreVerticalPosition();
            }
        }

        if(scanLineCycle_ == WORK_BEGIN_CYCLE) {
            if (scanLine_ == IDLE_BEGIN_SCANLINE + POST_RENDER_SCANLINES) {
                setVerticalBlanking();
            } else if (scanLine_ == PRERENDER_SCANLINE) {
                clearVerticalBlanking();
            }
        }

        goToNextDot();
    }

    uint8_t Ppu::read(uint16_t address) {
        auto selectedRegister = static_cast<Register>(address & REGISTER_ADDRESS_MASK);
        switch(selectedRegister) {
            case Register::STATUS:
                cpuDataBus_ = readStatus() | (cpuDataBus_ & 0x1f);
                break;
            case Register::OAM_DATA:
                cpuDataBus_ = readOamData();
                break;
            case Register::DATA:
                cpuDataBus_ = readData();
                break;
            case Register::CONTROL: // write-only registers
            case Register::MASK:
            case Register::OAM_ADDRESS:
            case Register::SCROLL:
            case Register::ADDRESS:
                break;
        }
        return cpuDataBus_;
    }

    uint8_t Ppu::readStatus() {
        // return status bits without affecting the unused bits in I/O bus
        auto status = *status_;
        isSecondWrite_ = false;
        status_.clear(StatusBit::VBLANK_STARTED);
        return status;
    }

    uint8_t Ppu::readOamData() {
        auto data = oamRam_[oamAddress_];
        if(!isVerticalBlanking_)
            oamAddress_++;
        return data;
    }

    uint8_t Ppu::readData() {
        auto byte = bus_.read(currentVRamAddress_);
        // return buffered byte if reading from VRAM. return immediate byte if reading the palette
        auto data = currentVRamAddress_ >= PaletteRamPort::PALETTE_START_ADDRESS ? byte : vRamData_;
        vRamData_ = byte;
        incrementVRamAddress();
        return data;
    }

    void Ppu::write(uint16_t address, uint8_t value) {
        cpuDataBus_ = value;
        auto selectedRegister = static_cast<Register>(address & REGISTER_ADDRESS_MASK);
        switch(selectedRegister) {
            case Register::CONTROL:
                return writeControl(value);
            case Register::MASK:
                return writeMask(value);
            case Register::OAM_ADDRESS:
                return writeOamAddress(value);
            case Register::OAM_DATA:
                return writeOamData(value);
            case Register::SCROLL:
                return writeScroll(value);
            case Register::ADDRESS:
                return writeAddress(value);
            case Register::DATA:
                return writeData(value);
            case Register::STATUS: // read-only register
                break;
        }
    }

    void Ppu::writeControl(uint8_t value) {
        auto previousInterruptDisabled = control_.isClear(ControlBit::VBLANK_INTERRUPT_ENABLE);
        *control_ = value;
        // set the nametable bits in the temporary VRam address
        tempVRamAddress_ = (tempVRamAddress_ & 0xf3ff) | ((value & 0x3) << 10);
        if(control_.isSet(ControlBit::VBLANK_INTERRUPT_ENABLE)
           && previousInterruptDisabled
           && status_.isSet(StatusBit::VBLANK_STARTED))
            verticalBlankInterrupt_.signal();
    }

    void Ppu::writeMask(uint8_t value) {
        *mask_ = value;
    }

    void Ppu::writeOamAddress(uint8_t value) {
        oamAddress_ = value;
    }

    void Ppu::writeOamData(uint8_t value) {
        oamRam_[oamAddress_] = value;
        oamAddress_++;
    }

    void Ppu::writeScroll(uint8_t value) {
        if(!isSecondWrite_) {
            // fill lower 5 bits of the address (coarse X)
            tempVRamAddress_ = (tempVRamAddress_ & 0xffe0) | (value >> 3);
            // lower 3 bits of the value is a fine X scroll
            fineScrollX_ = value & 0x7;
        } else {
            // erase coarse Y (bits 5-9) and fine scroll Y (bits 12-14)
            tempVRamAddress_ = (tempVRamAddress_ & 0x8c1f)
                                    // fill coarse Y
                                    | ((value & 0xf8) << 2)
                                    // fill fine scroll Y
                                    | ((value & 0x7) << 12);
        }
        isSecondWrite_ = !isSecondWrite_;
    }

    void Ppu::writeAddress(uint8_t value) {
        if(!isSecondWrite_) {
            // fill upper 6 bits
            tempVRamAddress_ = (value & 0x3f) << 8;
            // bit 15 is cleared for unknown reasons
            tempVRamAddress_ &= 0x3fff;
        } else {
            // fill lower 8 bits
            tempVRamAddress_ |= value;
            // copy to the current address register
            currentVRamAddress_ = tempVRamAddress_;
        }
        isSecondWrite_ = !isSecondWrite_;
    }

    void Ppu::writeData(uint8_t value) {
        bus_.write(currentVRamAddress_, value);
        incrementVRamAddress();
    }

    void Ppu::incrementVRamAddress() {
        if(isRendering()) {
            // emulate address increment quirk when rendering
            incrementCoarseX();
            incrementFineScrollY();
            return;
        }
        // increment normally
        auto incrementValue = control_.isSet(ControlBit::VRAM_ADDRESS_INCREMENT);
        if(!incrementValue)
            currentVRamAddress_++;
        else
            currentVRamAddress_ += 32;
    }

    // todo: fetching machine
    // input: nt addr, at addr, targetTile (inheritable struct), address evaluator?
    void Ppu::clockSpriteEvaluation() {
        /*if(scanLineCycle_ == OAM_INITIALIZE_BEGIN_CYCLE) {
            fillAllSpriteData();
        } else if(scanLineCycle_ >= SPRITE_FETCH_BEGIN_CYCLE && scanLineCycle_ <= SPRITE_FETCH_END_CYCLE) {
            if(scanLineCycle_ % 2 == 0) // each byte fetch takes 2 cycles
                return;
            // single sprite fetch phase takes 8 cycles. each two cycles a fetch is done
            auto fetchPhase = (scanLineCycle_ - 1) % 8 / 2;
            auto spriteIndex = (scanLineCycle_ - SPRITE_FETCH_BEGIN_CYCLE) / 8;
            auto& sprite = sprites_[spriteIndex];
            switch(fetchPhase) {
                case 0: // garbage nametable byte
                    bus_.read(NAMETABLE_START_ADDRESS);
                    break;
                case 1: // garbage attribute
                    bus_.read(NAMETABLE_START_ADDRESS + ATTRIBUTE_TABLE_OFFSET);
                    break;
                case 2: // sprite pattern least significant bits
                    sprite.lowerPattern = bus_.read(evaluateSpritePatternAddress(sprite, 0));
                    break;
                case 3: // sprite pattern most significant bits
                default:
                    sprite.upperPattern = bus_.read(evaluateSpritePatternAddress(sprite, 1));
                    break;
            }
        }*/
    }

    // todo: either directly copy OAM data into SpriteTile or break execution into single cycles
    void Ppu::fillAllSpriteData() {
        // fill the secondary OAM all at once
        // warning: this prevents from emulating OAM data access snooping
        std::fill(secondaryOamRam_.begin(), secondaryOamRam_.end(), 0xff);
        for(int i = oamAddress_; i < OAM_RAM_SIZE; i += OAM_ENTRY_SIZE) {
            auto slotOffset = spriteSlots_ * OAM_ENTRY_SIZE;
            auto verticalPosition = oamRam_[i];
            secondaryOamRam_[slotOffset] = verticalPosition;
            // will this sprite be rendered on the next scanline?
            if(verticalPosition <= scanLine_ + 1
                && verticalPosition + 8 * (1 + control_.isSet(ControlBit::SPRITE_SIZE)) >= scanLine_ + 1) {
                if(spriteSlots_ < MAX_SCANLINE_SPRITES) {
                    std::copy_n(std::begin(oamRam_) + i,
                                OAM_ENTRY_SIZE,
                                std::begin(secondaryOamRam_) + slotOffset);
                    spriteSlots_++;
                } else {
                    // warning: this does not emulate the sprite overflow bug
                    status_.set(StatusBit::SPRITE_OVERFLOW);
                }
            }
        }
        // transform OAM data into convenient form
        for(int i = 0; i < MAX_SCANLINE_SPRITES; i++) {
            auto& sprite = sprites_[i];
            fillSpriteData(&secondaryOamRam_[i * OAM_ENTRY_SIZE], sprites_[i]);
            ppu
        }
    }

    void Ppu::fillSpriteInitialData(const uint8_t* oamEntry, SpriteTile& sprite) {
        sprite.verticalPosition = oamEntry[0];
        sprite.index = oamEntry[1];
        auto attributes = oamEntry[2];
        sprite.palette = attributes & 0x3;
        sprite.priority = (attributes & 0x20) ? SpriteTile::Priority::BACK : SpriteTile::Priority::FRONT;
        sprite.doFlipHorizontally = attributes & 0x40;
        sprite.doFlipHorizontally = attributes & 0x80;
        sprite.horizontalPosition = oamEntry[3];
    }

    bool Ppu::fetchTileData(FetchPhase phase) {
        switch(phase) {
            case FetchPhase::NAMETABLE: {
                auto address = evaluateBackgroundNametableAddress();
                tempTileIndex_ = bus_.read(address);
                break;
            }
            case FetchPhase::ATTRIBUTE: {
                auto address = evaluateBackgroundAttributeAddress();
                tempAttributes_ = bus_.read(address);
                break;
            }
            case FetchPhase::PATTERN_LSB: {
                auto address = evaluateBackgroundPatternAddress(tempTileIndex_, 0);
                tempLowerPattern_ = bus_.read(address);
                break;
            }
            case FetchPhase::PATTERN_MSB: {
                auto address = evaluateBackgroundPatternAddress(tempTileIndex_, 1);
                tempUpperPattern_ = bus_.read(address);
                return true;
            }
        }
        return false;
    }

    bool Ppu::fetchSpriteData(FetchPhase phase, SpriteTile& sprite) {
        switch(phase) {
            case FetchPhase::NAMETABLE: {
                auto address = evaluateBackgroundNametableAddress();
                bus_.read(address);
                break;
            }
            case FetchPhase::ATTRIBUTE: {
                auto address = evaluateBackgroundAttributeAddress();
                bus_.read(address);
                break;
            }
            case FetchPhase::PATTERN_LSB: {
                auto address = evaluateSpritePatternAddress(sprite.verticalPosition, sprite.index, 0);
                sprite.lowerPattern = bus_.read(address);
                break;
            }
            case FetchPhase::PATTERN_MSB: {
                auto address = evaluateSpritePatternAddress(sprite.verticalPosition, sprite.index, 0);
                sprite.upperPattern = bus_.read(address);
                return true;
            }
        }
        return false;
    }

    void Ppu::outputPixel(uint8_t patternLsb, uint8_t patternMsb, uint8_t attributes) {
        auto pixel = patternLsb | (patternMsb << 1);
        screen_[screenOffset_] = pixel;
        screenOffset_ = (screenOffset_ + 1) % screen_.size();
    }

    uint16_t Ppu::evaluatePatternAddressBase(uint8_t verticalOffset,
                                             uint8_t plane,
                                             uint8_t index,
                                             uint8_t halfSelect) {
        return (verticalOffset & 0x7) | (plane << 3) | (index << 4) | (halfSelect << 12)
            | ((verticalOffset & 0x8) << 4); // 8x16 sprites
    }

    uint16_t Ppu::evaluateSpritePatternAddress(uint8_t verticalOffset, uint8_t index, uint8_t plane) const {
        bool use8x16Sprites = control_.isSet(ControlBit::SPRITE_SIZE);
        auto halfSelect = use8x16Sprites
                          ? index % 2
                          : control_.isSet(ControlBit::SPRITE_PATTERN_TABLE_ADDRESS);
        // when using 8x16 sprites, the first index bit is zero. its place is taken by vertical offset msb
        auto modifiedIndex = index & ~use8x16Sprites;
        return evaluatePatternAddressBase(verticalOffset, plane, modifiedIndex, halfSelect);
    }


    uint16_t Ppu::evaluateBackgroundPatternAddress(uint8_t index, uint8_t plane) const {
        auto halfSelect = control_.isSet(ControlBit::BACKGROUND_PATTERN_TABLE_ADDRESS);
        return evaluatePatternAddressBase(currentVRamAddress_ >> 12, plane, index, halfSelect);
    }

    uint16_t Ppu::evaluateBackgroundNametableAddress() const {
        return NAMETABLE_START_ADDRESS | (currentVRamAddress_ & 0x0fff);
    }

    uint16_t Ppu::evaluateBackgroundAttributeAddress() const {
        return (NAMETABLE_START_ADDRESS + ATTRIBUTE_TABLE_OFFSET)
               | (currentVRamAddress_ & 0x0c00)
               | ((currentVRamAddress_ >> 4) & 0x38)
               | ((currentVRamAddress_ >> 2) & 0x7);
    }

    bool Ppu::isRenderingEnabled() const {
        return mask_.isSet(MaskBit::SHOW_BACKGROUND) && mask_.isSet(MaskBit::SHOW_SPRITES);
    }

    bool Ppu::isRendering() const {
        return isRenderingEnabled()
                    && ((scanLine_ >= FIRST_VISIBLE_SCANLINE && scanLine_ <= LAST_VISIBLE_SCANLINE)
                        || scanLine_ == PRERENDER_SCANLINE);
    }

    /* https://www.nesdev.org/wiki/PPU_scrolling */
    void Ppu::incrementCoarseX() {
        if((currentVRamAddress_ & 0x001f) == 31) {
            // zero coarse X
            currentVRamAddress_ &= 0xffe0;
            // toggle horizontal nametable
            currentVRamAddress_ ^= 0x0400;
        } else {
            currentVRamAddress_++;
        }
    }

    /* https://www.nesdev.org/wiki/PPU_scrolling */
    void Ppu::incrementFineScrollY() {
        // increment fine scroll Y if it's not equal 7
        if((currentVRamAddress_ & 0x7000) != 0x7000) {
            currentVRamAddress_ += 0x1000;
        } else {
            // zero fine scroll Y
            currentVRamAddress_ &= 0x8fff;
            uint8_t coarseY = (currentVRamAddress_ >> 5) & 0x1f;
            if(coarseY == 29) {
                coarseY = 0;
                // toggle vertical nametable
                currentVRamAddress_ ^= 0x0800;
            } else if(coarseY == 31) {
                // no vertical nametable toggle (negative scroll)
                coarseY = 0;
            } else {
                coarseY++;
            }
            // emplace evaluated coarse Y into the current address
            currentVRamAddress_ = (currentVRamAddress_ & 0xfc1f) | (coarseY << 5);
        }
        // fineScrollY++;
        // coarseY += !!(fineScrollY & 0x8)
        // bool coarseYEnd = coarseY == 30
        // currentVRamAddress ^= coarseYEnd << 11;
        // fineScrollY &= 0x8;
        // coarseY &= 0x1f & ~(coarseYEnd * 0xff);
    }

    void Ppu::restoreHorizontalPosition() {
        currentVRamAddress_ = (currentVRamAddress_ & 0xfbe0) | (tempVRamAddress_ & 0x41f);
    }

    void Ppu::restoreVerticalPosition() {
        currentVRamAddress_ = tempVRamAddress_;
    }

    void Ppu::goToNextDot() {
        scanLineCycle_ = (scanLineCycle_ + 1) % SCANLINE_WIDTH;
        bool isNextScanLine = scanLineCycle_ == 0;
        scanLine_ = (scanLine_ + isNextScanLine) % TOTAL_SCANLINES;
        // todo: rewrite smarter
        // skip last frame dot on odd frame
        if(elapsedFrames_ % 2 == 1 && scanLine_ == PRERENDER_SCANLINE && scanLineCycle_ == SCANLINE_WIDTH - 2) {
            scanLine_ = 0;
            scanLineCycle_ = 0;
        }
        bool isNextFrame = isNextScanLine && scanLine_ == 0;
        elapsedFrames_ += isNextFrame;
    }

    bool Ppu::isVerticalBlanking() const {
        return isVerticalBlanking_;
    }

    void Ppu::setVerticalBlanking() {
        isVerticalBlanking_ = true;
        status_.set(StatusBit::VBLANK_STARTED);
        if(control_.isSet(ControlBit::VBLANK_INTERRUPT_ENABLE))
            verticalBlankInterrupt_.signal();
    }

    void Ppu::clearVerticalBlanking() {
        isVerticalBlanking_ = false;
        status_.clear(StatusBit::VBLANK_STARTED);
    }

} // ppu
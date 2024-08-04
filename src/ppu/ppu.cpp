//
// Created by Piotr on 19.09.2022.
//

#include "ppu/ppu.h"

#include "ppu/bus/paletteramport.h"
#include "util/bin.h"

using emu::util::bin::reverse;

namespace emu::ppu {

    Ppu::Ppu(PpuBus &bus, Wire verticalBlankInterrupt, std::span<uint8_t> screen)
        : scanLine_(PRERENDER_SCANLINE),
          scanLineCycle_(0),
          isVerticalBlanking_(false),
          elapsedFrames_(0),
          backgroundLowerQueue_(0x0),
          backgroundUpperQueue_(0x0),
          backgroundAttributesQueue_(0x0),
          backgroundAttributes_(),
          backgroundLowerPattern_(),
          backgroundUpperPattern_(),
          oamAddress_(0x0),
          spriteSlots_(0),
          isSecondWrite_(false),
          fineScrollX_(0x0),
          tempVRamAddress_(0x0),
          currentVRamAddress_(0x0),
          vRamData_(0x0),
          cpuDataBus_(0x0),
          oamRam_(),
          secondaryOamRam_(),
          sprites_(),
          bus_(bus),
          verticalBlankInterrupt_(verticalBlankInterrupt),
          screenOffset_(0),
          screen_(screen) {
        if(screen_.size() < SCREEN_WIDTH * SCREEN_HEIGHT)
            throw std::invalid_argument("the screen buffer is too small");
    }

    void Ppu::clock() {
        // fetch sprite/background tile every 8 ticks
        bool isFetchCycle = (scanLineCycle_ - WORK_BEGIN_CYCLE) % 8 == 0;

        if(isFetchCycle) {
            fillBackgroundRenderingBuffer();
        }

        if(scanLine_ <= LAST_VISIBLE_SCANLINE
           && scanLineCycle_ >= WORK_BEGIN_CYCLE && scanLineCycle_ <= RENDERING_END_CYCLE) {
            renderPixel();
            shiftBackgroundRenderingBuffer();
            decrementSpriteHorizontalPositions();
            screenOffset_++;
        }

        if(isRendering()) {
            // handle background fetches every 8 ticks
            // ofc don't overlap background fetches with sprite fetches
            if(!(scanLineCycle_ >= SPRITE_FETCH_BEGIN_CYCLE && scanLineCycle_ <= SPRITE_FETCH_END_CYCLE)) {
                if(isFetchCycle) {
                    bool doRedundantFetch = scanLineCycle_ == REDUNDANT_NT_FETCH_BEGIN_CYCLE;
                    fetchBackgroundTile(doRedundantFetch);
                } else if((scanLineCycle_ - WORK_BEGIN_CYCLE) % 8 == 7) {
                    // increment horizontal position just before a next fetch
                    incrementCoarseX();
                }
            }

            // modify current VRAM address accordingly
            if (scanLineCycle_ == VERTICAL_INCREMENT_CYCLE) {
                incrementFineScrollY();
            } else if (scanLineCycle_ == HORIZONTAL_RESTORE_CYCLE) {
                restoreHorizontalPosition();
                bool doSpriteEvaluation = scanLine_ != PRERENDER_SCANLINE;
                fetchSpriteTiles(doSpriteEvaluation);
            } else if (scanLine_ == PRERENDER_SCANLINE && scanLineCycle_ == VERTICAL_RESTORE_BEGIN_CYCLE) {
                restoreVerticalPosition();
            }
        }

        // update status & reset screen offset
        if(scanLineCycle_ == WORK_BEGIN_CYCLE) {
            if (scanLine_ == IDLE_BEGIN_SCANLINE + POST_RENDER_SCANLINES) {
                setVerticalBlanking();
            } else if (scanLine_ == PRERENDER_SCANLINE) {
                clearVerticalBlanking();
                status_.clear(StatusBit::SPRITE_0_HIT);
                status_.clear(StatusBit::SPRITE_OVERFLOW);
                screenOffset_ = 0;
            }
        }

        goToNextDot();
    }

    void Ppu::reset() {
        *control_ = 0x0;
        *mask_ = 0x0;
        isSecondWrite_ = false;
        vRamData_ = 0x0;
        elapsedFrames_ = 0;
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
        auto status = *status_;
        isSecondWrite_ = false;
        status_.clear(StatusBit::VBLANK_STARTED);
        return status;
    }

    uint8_t Ppu::readOamData() {
        auto data = oamRam_[oamAddress_];
        // address increments also do not occur when forced blanking, but no idea what it is
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
            // lower 3 bits of the value is the fine X scroll
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

    void Ppu::fetchBackgroundTile(bool doRedundantFetch) {
        if(doRedundantFetch) {
            auto address = evaluateBackgroundNametableAddress();
            bus_.read(address);
            bus_.read(address);
            return;
        }

        auto index = bus_.read(evaluateBackgroundNametableAddress());
        auto attributes = bus_.read(evaluateBackgroundAttributeAddress());
        // each attribute byte actually contains 4 palettes (one palette per one quadrant, a palette has 2 bits)
        // each quadrant consists of 4 tiles arranged in a 2x2 square
        auto quadrantSelect = (currentVRamAddress_ & 0x2) + ((currentVRamAddress_ >> 4) & 0x4);
        // select two attribute bits and then fill attribute buffer with duplicates of these bits
        uint16_t paletteBuffer = ((attributes >> quadrantSelect) & 0x3) * 0x5555;

        // push to queues
        backgroundAttributesQueue_ <<= 16;
        backgroundAttributesQueue_ |= paletteBuffer << (fineScrollX_ * 2);
        backgroundLowerQueue_ <<= 8;
        backgroundLowerQueue_ |= bus_.read(evaluateBackgroundPatternAddress(index, 0)) << fineScrollX_;
        backgroundUpperQueue_ <<= 8;
        backgroundUpperQueue_ |= bus_.read(evaluateBackgroundPatternAddress(index, 1)) << fineScrollX_;
    }

    void Ppu::fetchSpriteTiles(bool doSpriteEvaluation) {
        if(doSpriteEvaluation) {
            spriteSlots_ = 0;
            // todo: make oamAddress_ as initial i (unsafe accesses)
            //int i = oamAddress_;
            int i = 0, oamIndex = 0;
            for (; i < OAM_RAM_SIZE && spriteSlots_ < MAX_SCANLINE_SPRITES; i += OAM_ENTRY_SIZE) {
                auto verticalPosition = oamRam_[i];
                if (willRenderSprite(verticalPosition)) {
                    auto &sprite = sprites_[spriteSlots_];
                    sprite.order = oamIndex;
                    fillSpriteInitialData(&oamRam_[i], sprite);
                    spriteSlots_++;
                }
                oamIndex++;
            }
            // emulate sprite overflow bug
            for (; i < OAM_RAM_SIZE; i += OAM_ENTRY_SIZE + 1) {
                auto verticalPosition = oamRam_[i];
                if (willRenderSprite(verticalPosition))
                    status_.set(StatusBit::SPRITE_OVERFLOW);
            }
        }

        for(auto& sprite : sprites_)
            fillSpritePatternData(sprite);

        // initialize empty slots as transparent
        for(int i = spriteSlots_; i < MAX_SCANLINE_SPRITES; i++) {
            sprites_[i].lowerPattern = 0x0;
            sprites_[i].upperPattern = 0x0;
        }

    }

    bool Ppu::willRenderSprite(uint8_t verticalOffset) const {
        return verticalOffset <= scanLine_
                && verticalOffset + 8 * (1 + control_.isSet(ControlBit::SPRITE_SIZE)) - 1 >= scanLine_;
    }

    // todo: potentially unsafe memory accesses
    void Ppu::fillSpriteInitialData(const uint8_t* oamEntry, SpriteTile& sprite) {
        sprite.verticalPosition = oamEntry[0];
        sprite.index = oamEntry[1];
        auto attributes = oamEntry[2];
        sprite.palette = attributes & 0x3;
        sprite.priority = (attributes & 0x20) ? SpriteTile::Priority::BACK : SpriteTile::Priority::FRONT;
        sprite.doFlipHorizontally = attributes & 0x40;
        sprite.doFlipVertically = attributes & 0x80;
        sprite.horizontalPosition = oamEntry[3];
    }

    void Ppu::fillSpritePatternData(SpriteTile& sprite) {
        bus_.read(evaluateBackgroundNametableAddress()); // garbage nametable byte
        bus_.read(evaluateBackgroundAttributeAddress()); // garbage attribute byte
        // todo: reconsider moving this part into evaluateSpritePatternAddress() for clarity
        auto verticalOffset = scanLine_ - sprite.verticalPosition;
        if(sprite.doFlipVertically) {
            // todo: this branch can be eliminated. whenever SPRITE_SIZE gets updated, set subtrahend to 15 or 7 depending on bit value
            if(control_.isSet(ControlBit::SPRITE_SIZE))
                verticalOffset = 15 - verticalOffset;
            else
                verticalOffset = 7 - verticalOffset;
        }
        sprite.lowerPattern = bus_.read(evaluateSpritePatternAddress(verticalOffset, sprite.index, 0));
        sprite.upperPattern = bus_.read(evaluateSpritePatternAddress(verticalOffset, sprite.index, 1));
        if(sprite.doFlipHorizontally) {
            sprite.lowerPattern = reverse(sprite.lowerPattern);
            sprite.upperPattern = reverse(sprite.upperPattern);
        }
    }

    void Ppu::fillBackgroundRenderingBuffer() {
        backgroundLowerPattern_ = backgroundLowerQueue_ >> 8;
        backgroundUpperPattern_ = backgroundUpperQueue_ >> 8;
        backgroundAttributes_ = backgroundAttributesQueue_ >> 16;
    }

    void Ppu::renderPixel() {
        // todo: makePattern(lowerPattern, upperPattern)
        uint8_t lowerPattern = 0, upperPattern = 0, palette = 0;

        bool isRenderingRightSide = scanLineCycle_ > 8;
        bool isSpritePixel = false;

        if(mask_.isSet(MaskBit::SHOW_BACKGROUND)
                && (mask_.isSet(MaskBit::SHOW_LEFTMOST_BACKGROUND) || isRenderingRightSide)) {
            // todo: this can be optimized: shift by 6. then pass fully assembled pattern to outputPixel (1-2 ops less)
            lowerPattern = (backgroundLowerPattern_ & 0x80) >> 7;
            upperPattern = (backgroundUpperPattern_ & 0x80) >> 7;
            palette = backgroundAttributes_ >> 14;
        }

        auto sprite = findSprite();
        if(sprite
                && mask_.isSet(MaskBit::SHOW_SPRITES)
                && (mask_.isSet(MaskBit::SHOW_LEFTMOST_SPRITES) || isRenderingRightSide)) {
            // when an opaque sprite 0 pixel overlaps an opaque background pixel this is a sprite 0 hit
            // emulate hardware bug: hits not being detected at x=255
            bool isOpaqueBackground = lowerPattern || upperPattern;
            if(sprite->order == 0 && isOpaqueBackground && scanLineCycle_ != RENDERING_END_CYCLE)
                status_.set(StatusBit::SPRITE_0_HIT);
            if(sprite->priority == SpriteTile::Priority::FRONT || !isOpaqueBackground) {
                lowerPattern = (sprite->lowerPattern & 0x80) >> 7;
                upperPattern = (sprite->upperPattern & 0x80) >> 7;
                palette = sprite->palette;
                isSpritePixel = true;
            }
        }

        // if it is a transparent (empty) pixel, pick the universal backdrop color
        if(!lowerPattern && !upperPattern)
            palette = 0;

        outputPixel(lowerPattern, upperPattern, palette, isSpritePixel);
    }

    const Ppu::SpriteTile* Ppu::findSprite() const {
        for(auto& sprite : sprites_) {
            if(sprite.horizontalPosition == 0
                    && ((sprite.lowerPattern & 0x80) == 0x80 || (sprite.upperPattern & 0x80) == 0x80))
                return &sprite;
        }
        return nullptr;
    }

    void Ppu::decrementSpriteHorizontalPositions() {
        for(auto& sprite : sprites_) {
            if (sprite.horizontalPosition > 0) {
                sprite.horizontalPosition--;
            } else {
                sprite.lowerPattern <<= 1;
                sprite.upperPattern <<= 1;
            }
        }
    }

    void Ppu::shiftBackgroundRenderingBuffer() {
        backgroundAttributes_ <<= 2;
        backgroundLowerPattern_ <<= 1;
        backgroundUpperPattern_ <<= 1;
    }

    void Ppu::outputPixel(uint8_t patternLsb, uint8_t patternMsb, uint8_t palette, bool isSprite) {
        auto colorAddress = PaletteRamPort::PALETTE_START_ADDRESS
                            | patternLsb | (patternMsb << 1) | (palette << 2) | (isSprite << 4);
        auto pixel = bus_.read(colorAddress);
        if(mask_.isSet(MaskBit::GREYSCALE))
            pixel &= 0xf0;
        screen_[screenOffset_] = pixel;
    }

    uint16_t Ppu::evaluatePatternAddressBase(uint8_t verticalOffset,
                                             uint8_t plane,
                                             uint8_t index,
                                             uint8_t halfSelect) {
        return (verticalOffset & 0x7) | (plane << 3) | (index << 4) | (halfSelect << 12)
                | ((verticalOffset & 0x8) << 1); // 8x16 sprites
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
        auto offset = currentVRamAddress_ >> 12;
        return evaluatePatternAddressBase(offset, plane, index, halfSelect);
    }

    uint16_t Ppu::evaluateBackgroundNametableAddress() const {
        // add nametable offset and ignore fine scroll Y
        return NAMETABLE_START_ADDRESS | (currentVRamAddress_ & 0x0fff);
    }

    uint16_t Ppu::evaluateBackgroundAttributeAddress() const {
        return (NAMETABLE_START_ADDRESS + ATTRIBUTE_TABLE_OFFSET)
               // nametable select
               | (currentVRamAddress_ & 0x0c00)
               // high 3 bits of coarse Y
               | ((currentVRamAddress_ >> 4) & 0x38)
               // high 3 bits of coarse X
               | ((currentVRamAddress_ >> 2) & 0x7);
    }

    bool Ppu::isRenderingEnabled() const {
        return mask_.isSet(MaskBit::SHOW_BACKGROUND) || mask_.isSet(MaskBit::SHOW_SPRITES);
    }

    bool Ppu::isRendering() const {
        return isRenderingEnabled()
                    && ((scanLine_ >= FIRST_VISIBLE_SCANLINE && scanLine_ <= LAST_VISIBLE_SCANLINE)
                        || scanLine_ == PRERENDER_SCANLINE);
    }

    void Ppu::incrementVRamAddress() {
        if(isRendering()) {
            // emulate address increment quirk when rendering
            // presumably the coarse X is also incremented in the rendering phase...
            // battle city seems to write vRAM data when rendering
            incrementCoarseX(); // not sure about this...
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


    /* https://www.nesdev.org/wiki/PPU_scrolling */
    void Ppu::incrementCoarseX() {
        if((currentVRamAddress_ & 0x1f) == 31) {
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
        if(elapsedFrames_ % 2 == 1 && scanLine_ == PRERENDER_SCANLINE && scanLineCycle_ == SCANLINE_WIDTH - 1) {
            scanLine_ = 0;
            scanLineCycle_ = 0;
            isNextScanLine = true;
        }

        // is next frame?
        if(isNextScanLine && scanLine_ == 0) {
            elapsedFrames_++;
        }
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
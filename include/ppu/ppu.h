//
// Created by Piotr on 19.09.2022.
//

#ifndef EMU_PPU_H
#define EMU_PPU_H

#include <span>
#include <array>
#include <queue>

#include <cstdint>

#include "ppuregisters.h"
#include "bus/ppubus.h"
#include "memory/interfacememory.h"
#include "util/wire.h"
#include "util/bitregister.h"

namespace emu::cpu {

    class Cpu;

}

namespace emu::ppu {

    class Ppu final : public memory::InterfaceMemory<uint8_t, uint16_t> {
        using Wire = emu::util::Wire<emu::cpu::Cpu>;

        template<typename T>
        using BitRegister = emu::util::bin::BitRegister<T, uint8_t>;

    public:
        // scanlines
        static constexpr int TOTAL_SCANLINES = 262;
        static constexpr int FIRST_VISIBLE_SCANLINE = 0;
        static constexpr int LAST_VISIBLE_SCANLINE = 239;
        static constexpr int SCANLINE_WIDTH = 341;
        static constexpr int PRERENDER_SCANLINE = 261;

        static constexpr int IDLE_BEGIN_SCANLINE = 240;
        static constexpr int POST_RENDER_SCANLINES = 1;
        static constexpr int IDLE_END_SCANLINE = 260;

        // background cycles
        static constexpr int VERTICAL_INCREMENT_CYCLE = 256;
        static constexpr int HORIZONTAL_RESTORE_CYCLE = 257;
        static constexpr int REDUNDANT_NT_FETCH_BEGIN_CYCLE = 337;

        // rendering cycles
        static constexpr int WORK_BEGIN_CYCLE = 1;
        static constexpr int RENDERING_END_CYCLE = 256;

        // sprite cycles
        static constexpr int OAM_INITIALIZE_BEGIN_CYCLE = 1;
        static constexpr int SPRITE_FETCH_BEGIN_CYCLE = 257;
        static constexpr int SPRITE_FETCH_END_CYCLE = 320;
        static constexpr int VERTICAL_RESTORE_BEGIN_CYCLE = 280;

        // array sizes
        static constexpr std::size_t SCREEN_WIDTH = 256;
        static constexpr std::size_t SCREEN_HEIGHT = 240;

        static constexpr std::size_t OAM_RAM_SIZE = 256;
        static constexpr std::size_t SECONDARY_OAM_RAM_SIZE = 32;
        static constexpr std::size_t OAM_ENTRY_SIZE = 4;

        static constexpr int MAX_FRAME_SPRITES = 64;
        static constexpr int MAX_SCANLINE_SPRITES = 8;

        // masks
        static constexpr uint16_t REGISTER_ADDRESS_MASK = 0x7;

        // addresses
        static constexpr uint16_t NAMETABLE_START_ADDRESS = 0x2000;
        static constexpr uint16_t ATTRIBUTE_TABLE_OFFSET = 0x3c0;

    private:
        std::span<uint8_t> screen_;
        unsigned long screenOffset_;

        int scanLine_;
        int scanLineCycle_;
        bool isVerticalBlanking_;
        int elapsedFrames_;

        BitRegister<ControlBit> control_;
        BitRegister<MaskBit> mask_;
        BitRegister<StatusBit> status_;

        /* single pixel rendering buffers */
        uint16_t backgroundAttributes_;
        uint8_t backgroundLowerPattern_;
        uint8_t backgroundUpperPattern_;

        /* queued tile data */
        uint32_t backgroundAttributesQueue_;
        uint16_t backgroundLowerQueue_;
        uint16_t backgroundUpperQueue_;

        struct SpriteTile {
            int order;
            uint8_t verticalPosition;
            uint8_t index;
            uint8_t lowerPattern;
            uint8_t upperPattern;
            uint8_t palette;
            enum class Priority {
                FRONT=0,
                BACK
            } priority;
            bool doFlipHorizontally;
            bool doFlipVertically;
            uint8_t horizontalPosition;
        };

        std::array<SpriteTile, MAX_SCANLINE_SPRITES> sprites_;

        uint8_t oamAddress_;
        int spriteSlots_;
        std::array<uint8_t, OAM_RAM_SIZE> oamRam_;
        std::array<uint8_t, SECONDARY_OAM_RAM_SIZE> secondaryOamRam_;

        bool isSecondWrite_;

        uint8_t fineScrollX_;

        uint16_t tempVRamAddress_;
        uint16_t currentVRamAddress_;
        uint8_t vRamData_;

        uint8_t cpuDataBus_;
        PpuBus& bus_;
        Wire verticalBlankInterrupt_;

    public:
        explicit Ppu(PpuBus& bus, Wire verticalBlankInterrupt, std::span<uint8_t> screen);

        void clock();

        void reset();

        [[nodiscard]] bool isVerticalBlanking() const;

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;

    private:
        [[nodiscard]] uint8_t readStatus();
        [[nodiscard]] uint8_t readOamData();
        [[nodiscard]] uint8_t readData();

        void writeControl(uint8_t value);
        void writeMask(uint8_t value);
        void writeOamAddress(uint8_t value);
        void writeOamData(uint8_t value);
        void writeScroll(uint8_t value);
        void writeAddress(uint8_t value);
        void writeData(uint8_t value);

        void fetchBackgroundTile(bool doRedundantFetch);
        void fetchSpriteTiles(bool doSpriteEvaluation);
        [[nodiscard]] bool willRenderSprite(uint8_t verticalOffset) const;
        static void fillSpriteInitialData(const uint8_t* oamEntry, SpriteTile& sprite);
        void fillSpritePatternData(SpriteTile& sprite);

        void fillBackgroundRenderingBuffer();
        void renderPixel();
        [[nodiscard]] const SpriteTile* findSprite() const;
        void shiftBackgroundRenderingBuffer();
        void decrementSpriteHorizontalPositions();
        void outputPixel(uint8_t patternLsb, uint8_t patternMsb, uint8_t palette, bool isSprite);

        void incrementVRamAddress();
        void incrementCoarseX();
        void incrementFineScrollY();
        void restoreHorizontalPosition();
        void restoreVerticalPosition();
        void goToNextDot();

        void setVerticalBlanking();
        void clearVerticalBlanking();

        // todo:
        //[[nodiscard]] static uint8_t getPixelPattern(uint8_t lowerPattern, uint8_t upperPattern);
        //[[nodiscard]] static bool isOpaque(uint8_t pattern);

        [[nodiscard]] bool isRenderingEnabled() const;
        [[nodiscard]] bool isRendering() const;

        [[nodiscard]] static uint16_t evaluatePatternAddressBase(uint8_t verticalOffset,
                                                                 uint8_t plane,
                                                                 uint8_t index,
                                                                 uint8_t halfSelect);

        // todo: replace with TileInfo& struct: verticalOffset + index
        [[nodiscard]] uint16_t evaluateSpritePatternAddress(uint8_t verticalOffset, uint8_t index, uint8_t plane) const;
        [[nodiscard]] uint16_t evaluateBackgroundPatternAddress(uint8_t index, uint8_t plane) const;
        [[nodiscard]] uint16_t evaluateBackgroundNametableAddress() const;
        [[nodiscard]] uint16_t evaluateBackgroundAttributeAddress() const;
    };

} // ppu

#endif //EMU_PPU_H

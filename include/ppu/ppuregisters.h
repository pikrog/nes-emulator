//
// Created by Piotr on 20.09.2022.
//

#ifndef EMU_PPUREGISTERS_H
#define EMU_PPUREGISTERS_H

namespace emu::ppu {

    enum class Register {
        CONTROL=0x0,
        MASK,
        STATUS,
        OAM_ADDRESS,
        OAM_DATA,
        SCROLL,
        ADDRESS,
        DATA,
    };

    enum ControlBitValue {
        SPRITE_SIZE_8X8=0x0,
        SPRITE_SIZE_8X16=0x1,

        VRAM_ADDRESS_INCREMENT_1=0x0,
        VRAM_ADDRESS_INCREMENT_32=0x1,
    };

    enum class ControlBit {
        BASE_NAMETABLE_ADDRESS_LSB=0x1,
        BASE_NAMETABLE_ADDRESS_MSB=0x2,
        VRAM_ADDRESS_INCREMENT=0x4,
        SPRITE_PATTERN_TABLE_ADDRESS=0x8,
        BACKGROUND_PATTERN_TABLE_ADDRESS=0x10,
        SPRITE_SIZE=0x20,
        MASTER_SLAVE_SELECT=0x40,
        VBLANK_INTERRUPT_ENABLE=0x80,
    };

    enum class MaskBit {
        GREYSCALE=0x1,
        SHOW_LEFTMOST_BACKGROUND=0x2,
        SHOW_LEFTMOST_SPRITES=0x4,
        SHOW_BACKGROUND=0x8,
        SHOW_SPRITES=0x10,
        EMPHASIZE_RED=0x20,
        EMPHASIZE_GREEN=0x40,
        EMPHASIZE_BLUE=0x80
    };

    enum class StatusBit {
        SPRITE_OVERFLOW=0x20,
        SPRITE_0_HIT=0x40,
        VBLANK_STARTED=0x80,
    };

}

#endif //EMU_PPUREGISTERS_H

# NES Emulator

## General info

A simple NES Emulator. Features:
- core MOS 6502 (2A03) CPU emulation,
- PPU emulation (rendering graphics),
- reading INES & NES 2.0 (.nes) ROMs,
- NTSC mode only,
- supported mappers:
    - NROM (0),
- basic controls.

TODOs:
- full CPU emulation (with illegal opcodes),
- PAL and SECAM modes,
- APU emulation (with sound synthesis),
- support extra controllers (like zapper),
- graphical user interface,
- user config,
- optimize CPU and PPU emulation,
- improve portability.

![App screenshot](https://github.com/pikrog/nes-emulator/blob/main/screenshot.png?raw=true)

## Usage

    ./nes.exe <ROM_FILE>

//
// Created by Piotr on 31.08.2022.
//

// https://www.masswerk.at/6502/6502_instruction_set.html#RLA

#ifndef EMU_OPCODE_H
#define EMU_OPCODE_H

#include <cstdint>
#include "addressingmode.h"

namespace emu::cpu {

    enum class Operation {
        ADC,
        AND,
        ASL,
        BCC,
        BCS,
        BEQ,
        BIT,
        BMI,
        BNE,
        BPL,
        BRK,
        BVC,
        BVS,
        CLC,
        CLD,
        CLI,
        CLV,
        CMP,
        CPX,
        CPY,
        DEC,
        DEX,
        DEY,
        EOR,
        INC,
        INX,
        INY,
        JMP,
        JSR,
        LDA,
        LDX,
        LDY,
        LSR,
        NOP,
        ORA,
        PHA,
        PHP,
        PLA,
        PLP,
        ROL,
        ROR,
        RTI,
        RTS,
        SBC,
        SEC,
        SED,
        SEI,
        STA,
        STX,
        STY,
        TAX,
        TAY,
        TSX,
        TXA,
        TXS,
        TYA,

        /* unofficial mnemonics */
        ALR,
        ANC,
        ANE,
        ARR,
        DCP,
        ISC,
        JAM,
        LAS,
        LAX,
        LXA,
        RLA,
        RRA,
        SAX,
        SBX,
        SHA,
        SHX,
        SHY,
        SLO,
        SRE,
        TAS,
        USBC,
    };

    struct OpcodeAttributes {
        Operation operation;
        AddressingMode addressingMode;
        int cycles;
    };

    const OpcodeAttributes opcodes[256] = {
            {Operation::BRK, AddressingMode::IMPLICIT, 0}, // note: normally BRK takes 7 cycles
            {Operation::ORA, AddressingMode::INDIRECT_X, 6},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::SLO, AddressingMode::INDIRECT_X, 8},
            {Operation::NOP, AddressingMode::ZERO_PAGE, 3},
            {Operation::ORA, AddressingMode::ZERO_PAGE, 3},
            {Operation::ASL, AddressingMode::ZERO_PAGE, 5},
            {Operation::SLO, AddressingMode::ZERO_PAGE, 5},
            {Operation::PHP, AddressingMode::IMPLICIT, 3},
            {Operation::ORA, AddressingMode::IMMEDIATE, 2},
            {Operation::ASL, AddressingMode::ACCUMULATOR, 2},
            {Operation::ANC, AddressingMode::IMMEDIATE, 2},
            {Operation::NOP, AddressingMode::ABSOLUTE, 4},
            {Operation::ORA, AddressingMode::ABSOLUTE, 4},
            {Operation::ASL, AddressingMode::ABSOLUTE, 6},
            {Operation::SLO, AddressingMode::ABSOLUTE, 6},
            {Operation::BPL, AddressingMode::RELATIVE, 2},
            {Operation::ORA, AddressingMode::INDIRECT_Y, 5},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::SLO, AddressingMode::INDIRECT_Y, 8},
            {Operation::NOP, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::ORA, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::ASL, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::SLO, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::CLC, AddressingMode::IMPLICIT, 2},
            {Operation::ORA, AddressingMode::ABSOLUTE_Y, 4},
            {Operation::NOP, AddressingMode::IMPLICIT, 2},
            {Operation::SLO, AddressingMode::ABSOLUTE_Y, 7},
            {Operation::NOP, AddressingMode::ABSOLUTE_X, 4},
            {Operation::ORA, AddressingMode::ABSOLUTE_X, 4},
            {Operation::ASL, AddressingMode::ABSOLUTE_X, 7},
            {Operation::SLO, AddressingMode::ABSOLUTE_X, 7},
            {Operation::JSR, AddressingMode::ABSOLUTE, 6},
            {Operation::AND, AddressingMode::INDIRECT_X, 6},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::RLA, AddressingMode::INDIRECT_X, 8},
            {Operation::BIT, AddressingMode::ZERO_PAGE, 3},
            {Operation::AND, AddressingMode::ZERO_PAGE, 3},
            {Operation::ROL, AddressingMode::ZERO_PAGE, 5},
            {Operation::RLA, AddressingMode::ZERO_PAGE, 5},
            {Operation::PLP, AddressingMode::IMPLICIT, 4},
            {Operation::AND, AddressingMode::IMMEDIATE, 2},
            {Operation::ROL, AddressingMode::ACCUMULATOR, 2},
            {Operation::ANC, AddressingMode::IMMEDIATE, 2},
            {Operation::BIT, AddressingMode::ABSOLUTE, 4},
            {Operation::AND, AddressingMode::ABSOLUTE, 4},
            {Operation::ROL, AddressingMode::ABSOLUTE, 6},
            {Operation::RLA, AddressingMode::ABSOLUTE, 6},
            {Operation::BMI, AddressingMode::RELATIVE, 2},
            {Operation::AND, AddressingMode::INDIRECT_Y, 5},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::RLA, AddressingMode::INDIRECT_Y, 8},
            {Operation::NOP, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::AND, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::ROL, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::RLA, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::SEC, AddressingMode::IMPLICIT, 2},
            {Operation::AND, AddressingMode::ABSOLUTE_Y, 4},
            {Operation::NOP, AddressingMode::IMPLICIT, 2},
            {Operation::RLA, AddressingMode::ABSOLUTE_Y, 7},
            {Operation::NOP, AddressingMode::ABSOLUTE_X, 4},
            {Operation::AND, AddressingMode::ABSOLUTE_X, 4},
            {Operation::ROL, AddressingMode::ABSOLUTE_X, 7},
            {Operation::RLA, AddressingMode::ABSOLUTE_X, 7},
            {Operation::RTI, AddressingMode::IMPLICIT, 6},
            {Operation::EOR, AddressingMode::INDIRECT_X, 6},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::SRE, AddressingMode::INDIRECT_X, 8},
            {Operation::NOP, AddressingMode::ZERO_PAGE, 3},
            {Operation::EOR, AddressingMode::ZERO_PAGE, 3},
            {Operation::LSR, AddressingMode::ZERO_PAGE, 5},
            {Operation::SRE, AddressingMode::ZERO_PAGE, 5},
            {Operation::PHA, AddressingMode::IMPLICIT, 3},
            {Operation::EOR, AddressingMode::IMMEDIATE, 2},
            {Operation::LSR, AddressingMode::ACCUMULATOR, 2},
            {Operation::ALR, AddressingMode::IMMEDIATE, 2},
            {Operation::JMP, AddressingMode::ABSOLUTE, 3},
            {Operation::EOR, AddressingMode::ABSOLUTE, 4},
            {Operation::LSR, AddressingMode::ABSOLUTE, 6},
            {Operation::SRE, AddressingMode::ABSOLUTE, 6},
            {Operation::BVC, AddressingMode::RELATIVE, 2},
            {Operation::EOR, AddressingMode::INDIRECT_Y, 5},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::SRE, AddressingMode::INDIRECT_Y, 8},
            {Operation::NOP, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::EOR, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::LSR, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::SRE, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::CLI, AddressingMode::IMPLICIT, 2},
            {Operation::EOR, AddressingMode::ABSOLUTE_Y, 4},
            {Operation::NOP, AddressingMode::IMPLICIT, 2},
            {Operation::SRE, AddressingMode::ABSOLUTE_Y, 7},
            {Operation::NOP, AddressingMode::ABSOLUTE_X, 4},
            {Operation::EOR, AddressingMode::ABSOLUTE_X, 4},
            {Operation::LSR, AddressingMode::ABSOLUTE_X, 7},
            {Operation::SRE, AddressingMode::ABSOLUTE_X, 7},
            {Operation::RTS, AddressingMode::IMPLICIT, 6},
            {Operation::ADC, AddressingMode::INDIRECT_X, 6},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::RRA, AddressingMode::INDIRECT_X, 8},
            {Operation::NOP, AddressingMode::ZERO_PAGE, 3},
            {Operation::ADC, AddressingMode::ZERO_PAGE, 3},
            {Operation::ROR, AddressingMode::ZERO_PAGE, 5},
            {Operation::RRA, AddressingMode::ZERO_PAGE, 5},
            {Operation::PLA, AddressingMode::IMPLICIT, 4},
            {Operation::ADC, AddressingMode::IMMEDIATE, 2},
            {Operation::ROR, AddressingMode::ACCUMULATOR, 2},
            {Operation::ARR, AddressingMode::IMMEDIATE, 2},
            {Operation::JMP, AddressingMode::INDIRECT, 5},
            {Operation::ADC, AddressingMode::ABSOLUTE, 4},
            {Operation::ROR, AddressingMode::ABSOLUTE, 6},
            {Operation::RRA, AddressingMode::ABSOLUTE, 6},
            {Operation::BVS, AddressingMode::RELATIVE, 2},
            {Operation::ADC, AddressingMode::INDIRECT_Y, 5},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::RRA, AddressingMode::INDIRECT_Y, 8},
            {Operation::NOP, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::ADC, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::ROR, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::RRA, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::SEI, AddressingMode::IMPLICIT, 2},
            {Operation::ADC, AddressingMode::ABSOLUTE_Y, 4},
            {Operation::NOP, AddressingMode::IMPLICIT, 2},
            {Operation::RRA, AddressingMode::ABSOLUTE_Y, 7},
            {Operation::NOP, AddressingMode::ABSOLUTE_X, 4},
            {Operation::ADC, AddressingMode::ABSOLUTE_X, 4},
            {Operation::ROR, AddressingMode::ABSOLUTE_X, 7},
            {Operation::RRA, AddressingMode::ABSOLUTE_X, 7},
            {Operation::NOP, AddressingMode::IMMEDIATE, 2},
            {Operation::STA, AddressingMode::INDIRECT_X, 6},
            {Operation::NOP, AddressingMode::IMMEDIATE, 2},
            {Operation::SAX, AddressingMode::INDIRECT_X, 6},
            {Operation::STY, AddressingMode::ZERO_PAGE, 3},
            {Operation::STA, AddressingMode::ZERO_PAGE, 3},
            {Operation::STX, AddressingMode::ZERO_PAGE, 3},
            {Operation::SAX, AddressingMode::ZERO_PAGE, 3},
            {Operation::DEY, AddressingMode::IMPLICIT, 2},
            {Operation::NOP, AddressingMode::IMMEDIATE, 2},
            {Operation::TXA, AddressingMode::IMPLICIT, 2},
            {Operation::ANE, AddressingMode::IMMEDIATE, 2},
            {Operation::STY, AddressingMode::ABSOLUTE, 4},
            {Operation::STA, AddressingMode::ABSOLUTE, 4},
            {Operation::STX, AddressingMode::ABSOLUTE, 4},
            {Operation::SAX, AddressingMode::ABSOLUTE, 4},
            {Operation::BCC, AddressingMode::RELATIVE, 2},
            {Operation::STA, AddressingMode::INDIRECT_Y, 6},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::SHA, AddressingMode::INDIRECT_Y, 6},
            {Operation::STY, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::STA, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::STX, AddressingMode::ZERO_PAGE_Y, 4},
            {Operation::SAX, AddressingMode::ZERO_PAGE_Y, 4},
            {Operation::TYA, AddressingMode::IMPLICIT, 2},
            {Operation::STA, AddressingMode::ABSOLUTE_Y, 5},
            {Operation::TXS, AddressingMode::IMPLICIT, 2},
            {Operation::TAS, AddressingMode::ABSOLUTE_Y, 5},
            {Operation::SHY, AddressingMode::ABSOLUTE_X, 5},
            {Operation::STA, AddressingMode::ABSOLUTE_X, 5},
            {Operation::SHX, AddressingMode::ABSOLUTE_Y, 5},
            {Operation::SHA, AddressingMode::ABSOLUTE_Y, 5},
            {Operation::LDY, AddressingMode::IMMEDIATE, 2},
            {Operation::LDA, AddressingMode::INDIRECT_X, 6},
            {Operation::LDX, AddressingMode::IMMEDIATE, 2},
            {Operation::LAX, AddressingMode::INDIRECT_X, 6},
            {Operation::LDY, AddressingMode::ZERO_PAGE, 3},
            {Operation::LDA, AddressingMode::ZERO_PAGE, 3},
            {Operation::LDX, AddressingMode::ZERO_PAGE, 3},
            {Operation::LAX, AddressingMode::ZERO_PAGE, 3},
            {Operation::TAY, AddressingMode::IMPLICIT, 2},
            {Operation::LDA, AddressingMode::IMMEDIATE, 2},
            {Operation::TAX, AddressingMode::IMPLICIT, 2},
            {Operation::LXA, AddressingMode::IMMEDIATE, 2},
            {Operation::LDY, AddressingMode::ABSOLUTE, 4},
            {Operation::LDA, AddressingMode::ABSOLUTE, 4},
            {Operation::LDX, AddressingMode::ABSOLUTE, 4},
            {Operation::LAX, AddressingMode::ABSOLUTE, 4},
            {Operation::BCS, AddressingMode::RELATIVE, 2},
            {Operation::LDA, AddressingMode::INDIRECT_Y, 5},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::LAX, AddressingMode::INDIRECT_Y, 5},
            {Operation::LDY, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::LDA, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::LDX, AddressingMode::ZERO_PAGE_Y, 4},
            {Operation::LAX, AddressingMode::ZERO_PAGE_Y, 4},
            {Operation::CLV, AddressingMode::IMPLICIT, 2},
            {Operation::LDA, AddressingMode::ABSOLUTE_Y, 4},
            {Operation::TSX, AddressingMode::IMPLICIT, 2},
            {Operation::LAS, AddressingMode::ABSOLUTE_Y, 4},
            {Operation::LDY, AddressingMode::ABSOLUTE_X, 4},
            {Operation::LDA, AddressingMode::ABSOLUTE_X, 4},
            {Operation::LDX, AddressingMode::ABSOLUTE_Y, 4},
            {Operation::LAX, AddressingMode::ABSOLUTE_Y, 4},
            {Operation::CPY, AddressingMode::IMMEDIATE, 2},
            {Operation::CMP, AddressingMode::INDIRECT_X, 6},
            {Operation::NOP, AddressingMode::IMMEDIATE, 2},
            {Operation::DCP, AddressingMode::INDIRECT_X, 8},
            {Operation::CPY, AddressingMode::ZERO_PAGE, 3},
            {Operation::CMP, AddressingMode::ZERO_PAGE, 3},
            {Operation::DEC, AddressingMode::ZERO_PAGE, 5},
            {Operation::DCP, AddressingMode::ZERO_PAGE, 5},
            {Operation::INY, AddressingMode::IMPLICIT, 2},
            {Operation::CMP, AddressingMode::IMMEDIATE, 2},
            {Operation::DEX, AddressingMode::IMPLICIT, 2},
            {Operation::SBX, AddressingMode::IMMEDIATE, 2},
            {Operation::CPY, AddressingMode::ABSOLUTE, 4},
            {Operation::CMP, AddressingMode::ABSOLUTE, 4},
            {Operation::DEC, AddressingMode::ABSOLUTE, 6},
            {Operation::DCP, AddressingMode::ABSOLUTE, 6},
            {Operation::BNE, AddressingMode::RELATIVE, 2},
            {Operation::CMP, AddressingMode::INDIRECT_Y, 5},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::DCP, AddressingMode::INDIRECT_Y, 8},
            {Operation::NOP, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::CMP, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::DEC, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::DCP, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::CLD, AddressingMode::IMPLICIT, 2},
            {Operation::CMP, AddressingMode::ABSOLUTE_Y, 4},
            {Operation::NOP, AddressingMode::IMPLICIT, 2},
            {Operation::DCP, AddressingMode::ABSOLUTE_Y, 7},
            {Operation::NOP, AddressingMode::ABSOLUTE_X, 4},
            {Operation::CMP, AddressingMode::ABSOLUTE_X, 4},
            {Operation::DEC, AddressingMode::ABSOLUTE_X, 7},
            {Operation::DCP, AddressingMode::ABSOLUTE_X, 7},
            {Operation::CPX, AddressingMode::IMMEDIATE, 2},
            {Operation::SBC, AddressingMode::INDIRECT_X, 6},
            {Operation::NOP, AddressingMode::IMMEDIATE, 2},
            {Operation::ISC, AddressingMode::INDIRECT_X, 8},
            {Operation::CPX, AddressingMode::ZERO_PAGE, 3},
            {Operation::SBC, AddressingMode::ZERO_PAGE, 3},
            {Operation::INC, AddressingMode::ZERO_PAGE, 5},
            {Operation::ISC, AddressingMode::ZERO_PAGE, 5},
            {Operation::INX, AddressingMode::IMPLICIT, 2},
            {Operation::SBC, AddressingMode::IMMEDIATE, 2},
            {Operation::NOP, AddressingMode::IMPLICIT, 2},
            {Operation::USBC, AddressingMode::IMMEDIATE, 2},
            {Operation::CPX, AddressingMode::ABSOLUTE, 4},
            {Operation::SBC, AddressingMode::ABSOLUTE, 4},
            {Operation::INC, AddressingMode::ABSOLUTE, 6},
            {Operation::ISC, AddressingMode::ABSOLUTE, 6},
            {Operation::BEQ, AddressingMode::RELATIVE, 2},
            {Operation::SBC, AddressingMode::INDIRECT_Y, 5},
            {Operation::JAM, AddressingMode::UNSPECIFIED, 0},
            {Operation::ISC, AddressingMode::INDIRECT_Y, 8},
            {Operation::NOP, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::SBC, AddressingMode::ZERO_PAGE_X, 4},
            {Operation::INC, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::ISC, AddressingMode::ZERO_PAGE_X, 6},
            {Operation::SED, AddressingMode::IMPLICIT, 2},
            {Operation::SBC, AddressingMode::ABSOLUTE_Y, 4},
            {Operation::NOP, AddressingMode::IMPLICIT, 2},
            {Operation::ISC, AddressingMode::ABSOLUTE_Y, 7},
            {Operation::NOP, AddressingMode::ABSOLUTE_X, 4},
            {Operation::SBC, AddressingMode::ABSOLUTE_X, 4},
            {Operation::INC, AddressingMode::ABSOLUTE_X, 7},
            {Operation::ISC, AddressingMode::ABSOLUTE_X, 7},
    };
}

#endif //EMU_OPCODE_H

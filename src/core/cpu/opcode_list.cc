#include "cpu.h"

Opcode CPU::opcodeList[256] = {
    {"BRK",  AM_IMPL, &CPU::BRK, 7},
    {"ORA",  AM_XIND, &CPU::ORA, 6},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"SLO",  AM_XIND, &CPU::SLO, 8},
    {"NOP",  AM_ZP,   &CPU::NOP, 3},
    {"ORA",  AM_ZP,   &CPU::ORA, 3},
    {"ASL",  AM_ZP,   &CPU::ASL, 5},
    {"SLO",  AM_ZP,   &CPU::SLO, 5},
    {"PHP",  AM_IMPL, &CPU::PHP, 3},
    {"ORA",  AM_IMM,  &CPU::ORA, 2},
    {"ASL",  AM_ACC,  &CPU::ASL, 2},
    {"ANC",  AM_IMM,  &CPU::ANC, 2},
    {"NOP",  AM_ABS,  &CPU::NOP, 4},
    {"ORA",  AM_ABS,  &CPU::ORA, 4},
    {"ASL",  AM_ABS,  &CPU::ASL, 6},
    {"SLO",  AM_ABS,  &CPU::SLO, 6},
    {"BPL",  AM_REL,  &CPU::BPL, 2},
    {"ORA",  AM_INDY, &CPU::ORA, 5},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"SLO",  AM_INDY, &CPU::SLO, 8},
    {"NOP",  AM_ZPX,  &CPU::NOP, 4},
    {"ORA",  AM_ZPX,  &CPU::ORA, 4},
    {"ASL",  AM_ZPX,  &CPU::ASL, 6},
    {"SLO",  AM_ZPX,  &CPU::SLO, 6},
    {"CLC",  AM_IMPL, &CPU::CLC, 2},
    {"ORA",  AM_ABSY, &CPU::ORA, 4},
    {"NOP",  AM_IMPL, &CPU::NOP, 2},
    {"SLO",  AM_ABSY, &CPU::SLO, 7},
    {"NOP",  AM_ABSX, &CPU::NOP, 4},
    {"ORA",  AM_ABSX, &CPU::ORA, 4},
    {"ASL",  AM_ABSX, &CPU::ASL, 7},
    {"SLO",  AM_ABSX, &CPU::SLO, 7},
    {"JSR",  AM_ABS,  &CPU::JSR, 6},
    {"AND",  AM_XIND, &CPU::AND, 6},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"RLA",  AM_XIND, &CPU::RLA, 8},
    {"BIT",  AM_ZP,   &CPU::BIT, 3},
    {"AND",  AM_ZP,   &CPU::AND, 3},
    {"ROL",  AM_ZP,   &CPU::ROL, 5},
    {"RLA",  AM_ZP,   &CPU::RLA, 5},
    {"PLP",  AM_IMPL, &CPU::PLP, 4},
    {"AND",  AM_IMM,  &CPU::AND, 2},
    {"ROL",  AM_ACC,  &CPU::ROL, 2},
    {"ANC",  AM_IMM,  &CPU::ANC, 2},
    {"BIT",  AM_ABS,  &CPU::BIT, 4},
    {"AND",  AM_ABS,  &CPU::AND, 4},
    {"ROL",  AM_ABS,  &CPU::ROL, 6},
    {"RLA",  AM_ABS,  &CPU::RLA, 6},
    {"BMI",  AM_REL,  &CPU::BMI, 2},
    {"AND",  AM_INDY, &CPU::AND, 5},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"RLA",  AM_INDY, &CPU::RLA, 8},
    {"NOP",  AM_ZPX,  &CPU::NOP, 4},
    {"AND",  AM_ZPX,  &CPU::AND, 4},
    {"ROL",  AM_ZPX,  &CPU::ROL, 6},
    {"RLA",  AM_ZPX,  &CPU::RLA, 6},
    {"SEC",  AM_IMPL, &CPU::SEC, 2},
    {"AND",  AM_ABSY, &CPU::AND, 4},
    {"NOP",  AM_IMPL, &CPU::NOP, 2},
    {"RLA",  AM_ABSY, &CPU::RLA, 7},
    {"NOP",  AM_ABSX, &CPU::NOP, 4},
    {"AND",  AM_ABSX, &CPU::AND, 4},
    {"ROL",  AM_ABSX, &CPU::ROL, 7},
    {"RLA",  AM_ABSX, &CPU::RLA, 7},
    {"RTI",  AM_IMPL, &CPU::RTI, 6},
    {"EOR",  AM_XIND, &CPU::EOR, 6},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"SRE",  AM_XIND, &CPU::SRE, 8},
    {"NOP",  AM_ZP,   &CPU::NOP, 3},
    {"EOR",  AM_ZP,   &CPU::EOR, 3},
    {"LSR",  AM_ZP,   &CPU::LSR, 5},
    {"SRE",  AM_ZP,   &CPU::SRE, 5},
    {"PHA",  AM_IMPL, &CPU::PHA, 3},
    {"EOR",  AM_IMM,  &CPU::EOR, 2},
    {"LSR",  AM_ACC,  &CPU::LSR, 2},
    {"ALR",  AM_IMM,  &CPU::ALR, 2},
    {"JMP",  AM_ABS,  &CPU::JMP, 3},
    {"EOR",  AM_ABS,  &CPU::EOR, 4},
    {"LSR",  AM_ABS,  &CPU::LSR, 6},
    {"SRE",  AM_ABS,  &CPU::SRE, 6},
    {"BVC",  AM_REL,  &CPU::BVC, 2},
    {"EOR",  AM_INDY, &CPU::EOR, 5},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"SRE",  AM_INDY, &CPU::SRE, 8},
    {"NOP",  AM_ZPX,  &CPU::NOP, 4},
    {"EOR",  AM_ZPX,  &CPU::EOR, 4},
    {"LSR",  AM_ZPX,  &CPU::LSR, 6},
    {"SRE",  AM_ZPX,  &CPU::SRE, 6},
    {"CLI",  AM_IMPL, &CPU::CLI, 2},
    {"EOR",  AM_ABSY, &CPU::EOR, 4},
    {"NOP",  AM_IMPL, &CPU::NOP, 2},
    {"SRE",  AM_ABSY, &CPU::SRE, 7},
    {"NOP",  AM_ABSX, &CPU::NOP, 4},
    {"EOR",  AM_ABSX, &CPU::EOR, 4},
    {"LSR",  AM_ABSX, &CPU::LSR, 7},
    {"SRE",  AM_ABSX, &CPU::SRE, 7},
    {"RTS",  AM_IMPL, &CPU::RTS, 6},
    {"ADC",  AM_XIND, &CPU::ADC, 6},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"RRA",  AM_XIND, &CPU::RRA, 8},
    {"NOP",  AM_ZP,   &CPU::NOP, 3},
    {"ADC",  AM_ZP,   &CPU::ADC, 3},
    {"ROR",  AM_ZP,   &CPU::ROR, 5},
    {"RRA",  AM_ZP,   &CPU::RRA, 5},
    {"PLA",  AM_IMPL, &CPU::PLA, 4},
    {"ADC",  AM_IMM,  &CPU::ADC, 2},
    {"ROR",  AM_ACC,  &CPU::ROR, 2},
    {"ARR",  AM_IMM,  &CPU::ARR, 2},
    {"JMP",  AM_IND,  &CPU::JMP, 5},
    {"ADC",  AM_ABS,  &CPU::ADC, 4},
    {"ROR",  AM_ABS,  &CPU::ROR, 6},
    {"RRA",  AM_ABS,  &CPU::RRA, 6},
    {"BVS",  AM_REL,  &CPU::BVS, 2},
    {"ADC",  AM_INDY, &CPU::ADC, 5},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"RRA",  AM_INDY, &CPU::RRA, 8},
    {"NOP",  AM_ZPX,  &CPU::NOP, 4},
    {"ADC",  AM_ZPX,  &CPU::ADC, 4},
    {"ROR",  AM_ZPX,  &CPU::ROR, 6},
    {"RRA",  AM_ZPX,  &CPU::RRA, 6},
    {"SEI",  AM_IMPL, &CPU::SEI, 2},
    {"ADC",  AM_ABSY, &CPU::ADC, 4},
    {"NOP",  AM_IMPL, &CPU::NOP, 2},
    {"RRA",  AM_ABSY, &CPU::RRA, 7},
    {"NOP",  AM_ABSX, &CPU::NOP, 4},
    {"ADC",  AM_ABSX, &CPU::ADC, 4},
    {"ROR",  AM_ABSX, &CPU::ROR, 7},
    {"RRA",  AM_ABSX, &CPU::RRA, 7},
    {"NOP",  AM_IMM,  &CPU::NOP, 2},
    {"STA",  AM_XIND, &CPU::STA, 6},
    {"NOP",  AM_IMM,  &CPU::NOP, 2},
    {"SAX",  AM_XIND, &CPU::SAX, 6},
    {"STY",  AM_ZP,   &CPU::STY, 3},
    {"STA",  AM_ZP,   &CPU::STA, 3},
    {"STX",  AM_ZP,   &CPU::STX, 3},
    {"SAX",  AM_ZP,   &CPU::SAX, 3},
    {"DEY",  AM_IMPL, &CPU::DEY, 2},
    {"NOP",  AM_IMM,  &CPU::NOP, 2},
    {"TXA",  AM_IMPL, &CPU::TXA, 2},
    {"ANE",  AM_IMM,  &CPU::ANE, 0},
    {"STY",  AM_ABS,  &CPU::STY, 4},
    {"STA",  AM_ABS,  &CPU::STA, 4},
    {"STX",  AM_ABS,  &CPU::STX, 4},
    {"SAX",  AM_ABS,  &CPU::SAX, 4},
    {"BCC",  AM_REL,  &CPU::BCC, 2},
    {"STA",  AM_INDY, &CPU::STA, 6},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"SHA",  AM_INDY, &CPU::SHA, 0},
    {"STY",  AM_ZPX,  &CPU::STY, 4},
    {"STA",  AM_ZPX,  &CPU::STA, 4},
    {"STX",  AM_ZPY,  &CPU::STX, 4},
    {"SAX",  AM_ZPY,  &CPU::SAX, 4},
    {"TYA",  AM_IMPL, &CPU::TYA, 2},
    {"STA",  AM_ABSY, &CPU::STA, 5},
    {"TXS",  AM_IMPL, &CPU::TXS, 2},
    {"TAS",  AM_ABSY, &CPU::TAS, 0},
    {"SHY",  AM_ABSX, &CPU::SHY, 0},
    {"STA",  AM_ABSX, &CPU::STA, 5},
    {"SHX",  AM_ABSY, &CPU::SHX, 0},
    {"SHA",  AM_ABSY, &CPU::SHA, 0},
    {"LDY",  AM_IMM,  &CPU::LDY, 2},
    {"LDA",  AM_XIND, &CPU::LDA, 6},
    {"LDX",  AM_IMM,  &CPU::LDX, 2},
    {"LAX",  AM_XIND, &CPU::LAX, 6},
    {"LDY",  AM_ZP,   &CPU::LDY, 3},
    {"LDA",  AM_ZP,   &CPU::LDA, 3},
    {"LDX",  AM_ZP,   &CPU::LDX, 3},
    {"LAX",  AM_ZP,   &CPU::LAX, 3},
    {"TAY",  AM_IMPL, &CPU::TAY, 2},
    {"LDA",  AM_IMM,  &CPU::LDA, 2},
    {"TAX",  AM_IMPL, &CPU::TAX, 2},
    {"LXA",  AM_IMM,  &CPU::LXA, 0},
    {"LDY",  AM_ABS,  &CPU::LDY, 4},
    {"LDA",  AM_ABS,  &CPU::LDA, 4},
    {"LDX",  AM_ABS,  &CPU::LDX, 4},
    {"LAX",  AM_ABS,  &CPU::LAX, 4},
    {"BCS",  AM_REL,  &CPU::BCS, 2},
    {"LDA",  AM_INDY, &CPU::LDA, 5},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"LAX",  AM_INDY, &CPU::LAX, 5},
    {"LDY",  AM_ZPX,  &CPU::LDY, 4},
    {"LDA",  AM_ZPX,  &CPU::LDA, 4},
    {"LDX",  AM_ZPY,  &CPU::LDX, 4},
    {"LAX",  AM_ZPY,  &CPU::LAX, 4},
    {"CLV",  AM_IMPL, &CPU::CLV, 2},
    {"LDA",  AM_ABSY, &CPU::LDA, 4},
    {"TSX",  AM_IMPL, &CPU::TSX, 2},
    {"LAS",  AM_ABSY, &CPU::LAS, 4},
    {"LDY",  AM_ABSX, &CPU::LDY, 4},
    {"LDA",  AM_ABSX, &CPU::LDA, 4},
    {"LDX",  AM_ABSY, &CPU::LDX, 4},
    {"LAX",  AM_ABSY, &CPU::LAX, 4},
    {"CPY",  AM_IMM,  &CPU::CPY, 2},
    {"CMP",  AM_XIND, &CPU::CMP, 6},
    {"NOP",  AM_IMM,  &CPU::NOP, 2},
    {"DCP",  AM_XIND, &CPU::DCP, 8},
    {"CPY",  AM_ZP,   &CPU::CPY, 3},
    {"CMP",  AM_ZP,   &CPU::CMP, 3},
    {"DEC",  AM_ZP,   &CPU::DEC, 5},
    {"DCP",  AM_ZP,   &CPU::DCP, 5},
    {"INY",  AM_IMPL, &CPU::INY, 2},
    {"CMP",  AM_IMM,  &CPU::CMP, 2},
    {"DEX",  AM_IMPL, &CPU::DEX, 2},
    {"SBX",  AM_IMM,  &CPU::SBX, 2},
    {"CPY",  AM_ABS,  &CPU::CPY, 4},
    {"CMP",  AM_ABS,  &CPU::CMP, 4},
    {"DEC",  AM_ABS,  &CPU::DEC, 6},
    {"DCP",  AM_ABS,  &CPU::DCP, 6},
    {"BNE",  AM_REL,  &CPU::BNE, 2},
    {"CMP",  AM_INDY, &CPU::CMP, 5},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"DCP",  AM_INDY, &CPU::DCP, 8},
    {"NOP",  AM_ZPX,  &CPU::NOP, 4},
    {"CMP",  AM_ZPX,  &CPU::CMP, 4},
    {"DEC",  AM_ZPX,  &CPU::DEC, 6},
    {"DCP",  AM_ZPX,  &CPU::DCP, 6},
    {"CLD",  AM_IMPL, &CPU::CLD, 2},
    {"CMP",  AM_ABSY, &CPU::CMP, 4},
    {"NOP",  AM_IMPL, &CPU::NOP, 2},
    {"DCP",  AM_ABSY, &CPU::DCP, 7},
    {"NOP",  AM_ABSX, &CPU::NOP, 4},
    {"CMP",  AM_ABSX, &CPU::CMP, 4},
    {"DEC",  AM_ABSX, &CPU::DEC, 7},
    {"DCP",  AM_ABSX, &CPU::DCP, 7},
    {"CPX",  AM_IMM,  &CPU::CPX, 2},
    {"SBC",  AM_XIND, &CPU::SBC, 6},
    {"NOP",  AM_IMM,  &CPU::NOP, 2},
    {"ISC",  AM_XIND, &CPU::ISC, 8},
    {"CPX",  AM_ZP,   &CPU::CPX, 3},
    {"SBC",  AM_ZP,   &CPU::SBC, 3},
    {"INC",  AM_ZP,   &CPU::INC, 5},
    {"ISC",  AM_ZP,   &CPU::ISC, 5},
    {"INX",  AM_IMPL, &CPU::INX, 2},
    {"SBC",  AM_IMM,  &CPU::SBC, 2},
    {"NOP",  AM_IMPL, &CPU::NOP, 2},
    {"USBC", AM_IMM,  &CPU::USBC, 2},
    {"CPX",  AM_ABS,  &CPU::CPX, 4},
    {"SBC",  AM_ABS,  &CPU::SBC, 4},
    {"INC",  AM_ABS,  &CPU::INC, 6},
    {"ISC",  AM_ABS,  &CPU::ISC, 6},
    {"BEQ",  AM_REL,  &CPU::BEQ, 2},
    {"SBC",  AM_INDY, &CPU::SBC, 5},
    {"JAM",  AM_ILL,  &CPU::JAM, 11},
    {"ISC",  AM_INDY, &CPU::ISC, 8},
    {"NOP",  AM_ZPX,  &CPU::NOP, 4},
    {"SBC",  AM_ZPX,  &CPU::SBC, 4},
    {"INC",  AM_ZPX,  &CPU::INC, 6},
    {"ISC",  AM_ZPX,  &CPU::ISC, 6},
    {"SED",  AM_IMPL, &CPU::SED, 2},
    {"SBC",  AM_ABSY, &CPU::SBC, 4},
    {"NOP",  AM_IMPL, &CPU::NOP, 2},
    {"ISC",  AM_ABSY, &CPU::ISC, 7},
    {"NOP",  AM_ABSX, &CPU::NOP, 4},
    {"SBC",  AM_ABSX, &CPU::SBC, 4},
    {"INC",  AM_ABSX, &CPU::INC, 7},
    {"ISC",  AM_ABSX, &CPU::ISC, 7},
}; 

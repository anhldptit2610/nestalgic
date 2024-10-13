#include "cpu/cpu.h"

/* Load/Store Opcodes */
void CPU::LDA()
{
    switch (instr.addressingMode) {
    case AM_IMM:
        regs.A = instr.operand[0];
        break;
    case AM_ABSX:
    case AM_ABSY:
    case AM_INDY:
        instr.bonusCycles = (instr.pageCrossed) ? 1 : 0;
    default:
        regs.A = ReadByte(instr.effectiveAddress);
        break;
    }

    SetNZFlag(regs.A);
}

void CPU::LDX()
{
    switch (instr.addressingMode) {
    case AM_IMM:
        regs.X = instr.operand[0];
        break;
    case AM_ABSY:
        instr.bonusCycles = (instr.pageCrossed) ? 1 : 0;
    default:
        regs.X = ReadByte(instr.effectiveAddress);
        break;
    }

    SetNZFlag(regs.X);
}

void CPU::LDY()
{
    switch (instr.addressingMode) {
    case AM_IMM:
        regs.Y = instr.operand[0];
        break;
    case AM_ABSX:
        instr.bonusCycles = (instr.pageCrossed) ? 1 : 0;
    default:
        regs.Y = ReadByte(instr.effectiveAddress);
        break;
    }

    SetNZFlag(regs.Y);
}

void CPU::STA()
{
    WriteByte(instr.effectiveAddress, regs.A);
}

void CPU::STX()
{
    WriteByte(instr.effectiveAddress, regs.X);
}

void CPU::STY()
{
    WriteByte(instr.effectiveAddress, regs.Y);
}

void CPU::TAX()
{
    regs.X = regs.A;
    SetNZFlag(regs.X);
}

void CPU::TAY()
{
    regs.Y = regs.A;
    SetNZFlag(regs.Y);
}

void CPU::TXA()
{
    regs.A = regs.X;
    SetNZFlag(regs.A);
}

void CPU::TYA()
{
    regs.A = regs.Y;
    SetNZFlag(regs.A);
}

void CPU::TSX()
{
    regs.X = regs.S;
    SetNZFlag(regs.X);
}

void CPU::TXS()
{
    regs.S = regs.X;
}

void CPU::PHP()
{
    PushByte(flags.P | (1U << 4) | (1U << 5));
}

void CPU::PLA()
{
    regs.A = PopByte();
    SetNZFlag(regs.A);
}

void CPU::PLP()
{
    uint8_t p = PopByte();

    flags.C = NTHBIT(p, 0);
    flags.Z = NTHBIT(p, 1);
    flags.I = NTHBIT(p, 2);
    flags.D = NTHBIT(p, 3);
    flags.V = NTHBIT(p, 6);
    flags.N = NTHBIT(p, 7);
}

void CPU::AND()
{
    switch (instr.addressingMode) {
    case AM_IMM:
        regs.A &= instr.operand[0];
        break;
    case AM_ABSX:
    case AM_ABSY:
    case AM_INDY:
        instr.bonusCycles = (instr.pageCrossed &&
                            instr.opcode != 0x33 &&
                            instr.opcode != 0x3b &&
                            instr.opcode != 0x3f) ? 1 : 0;
    default:
        regs.A &= ReadByte(instr.effectiveAddress);
        break;
    }

    SetNZFlag(regs.A);
}

void CPU::EOR()
{
    switch (instr.addressingMode) {
    case AM_IMM:
        regs.A ^= instr.operand[0];
        break;
    case AM_ABSX:
    case AM_ABSY:
    case AM_INDY:
        instr.bonusCycles = (instr.pageCrossed &&
                            instr.opcode != 0x53 &&
                            instr.opcode != 0x5b &&
                            instr.opcode != 0x5f) ? 1 : 0;
    default:
        regs.A ^= ReadByte(instr.effectiveAddress);
        break;
    }

    SetNZFlag(regs.A);
}

void CPU::ORA()
{
    switch (instr.addressingMode) {
    case AM_IMM:
        regs.A |= instr.operand[0];
        break;
    case AM_ABSX:
    case AM_ABSY:
    case AM_INDY:
        instr.bonusCycles = (instr.pageCrossed && 
            instr.opcode != 0x13 &&
            instr.opcode != 0x1b &&
            instr.opcode != 0x1f) ? 1 : 0;
    default:
        regs.A |= ReadByte(instr.effectiveAddress);
        break;
    }

    SetNZFlag(regs.A);
}

void CPU::BIT()
{
    uint8_t operand = ReadByte(instr.effectiveAddress);
    flags.Z = !(regs.A & operand);
    flags.N = NTHBIT(operand, 7);
    flags.V = NTHBIT(operand, 6);
}

void CPU::ADC()
{
    uint8_t result, b;

    switch (instr.addressingMode) {
    case AM_IMM:
        b = instr.operand[0];
        break;
    case AM_ABSX:
    case AM_ABSY:
    case AM_INDY:
        instr.bonusCycles = (instr.pageCrossed &&
            instr.opcode != 0x73 &&
            instr.opcode != 0x7b &&
            instr.opcode != 0x7f) ? 1 : 0;
    default:
        b = ReadByte(instr.effectiveAddress);
        break;
    }

    result = regs.A + b + (uint8_t)flags.C;

    flags.Z = !result;
    flags.N = NTHBIT(result, 7);
    flags.C = ((uint16_t)regs.A + (uint16_t)b + (uint16_t)flags.C) >= 0x0100;
    flags.V = ((result ^ b) & (result ^ regs.A) & 0x80) > 0;

    regs.A = result;
}

void CPU::SBC()
{
    uint8_t result, b;

    switch (instr.addressingMode) {
    case AM_IMM:
        b = instr.operand[0];
        break;
    case AM_ABSX:
    case AM_ABSY:
    case AM_INDY:
        instr.bonusCycles = (instr.pageCrossed &&
            instr.opcode != 0xf3 &&
            instr.opcode != 0xfb &&
            instr.opcode != 0xff) ? 1 : 0;
    default:
        b = ReadByte(instr.effectiveAddress);
        break;
    }

    result = regs.A + ~b + (uint8_t)flags.C;

    flags.Z = !result;
    flags.N = NTHBIT(result, 7);
    flags.C = ((uint16_t)regs.A + (uint16_t)(b ^ 0xff) + (uint16_t)flags.C) >= 0x0100;
    flags.V = ((result ^ (~b)) & (result ^ regs.A) & 0x80) > 0;

    regs.A = result;
}

void CPU::CMP()
{
    uint8_t operand, result;

    switch (instr.addressingMode) {
    case AM_IMM:
        operand = instr.operand[0];
        break;
    case AM_ABSX:
    case AM_ABSY:
    case AM_INDY:
        instr.bonusCycles = (instr.pageCrossed && 
            instr.opcode != 0xd3 &&
            instr.opcode != 0xdb &&
            instr.opcode != 0xdf) ? 1 : 0;
    default:
        operand = ReadByte(instr.effectiveAddress);
        break;
    }

    result = regs.A - operand;

    flags.C = regs.A >= operand;
    flags.N = NTHBIT(result, 7);
    flags.Z = !result;
}

void CPU::CPX()
{
    uint8_t operand, result;

    switch (instr.addressingMode) {
    case AM_IMM:
        operand = instr.operand[0];
        break;
    default:
        operand = ReadByte(instr.effectiveAddress);
        break;
    }

    result = regs.X - operand;

    flags.C = regs.X >= operand;
    flags.N = NTHBIT(result, 7);
    flags.Z = !result;
}

void CPU::CPY()
{
    uint8_t operand, result;

    switch (instr.addressingMode) {
    case AM_IMM:
        operand = instr.operand[0];
        break;
    default:
        operand = ReadByte(instr.effectiveAddress);
        break;
    }

    result = regs.Y - operand;

    flags.C = regs.Y >= operand;
    flags.N = NTHBIT(result, 7);
    flags.Z = !result;
}

void CPU::INC()
{
    uint8_t mem = ReadByte(instr.effectiveAddress) + 1;

    WriteByte(instr.effectiveAddress, mem);
    SetNZFlag(mem);
}

void CPU::INX()
{
    regs.X++;
    SetNZFlag(regs.X);
}

void CPU::INY()
{
    regs.Y++;
    SetNZFlag(regs.Y);
}

void CPU::DEC()
{
    uint8_t mem = ReadByte(instr.effectiveAddress) - 1;

    WriteByte(instr.effectiveAddress, mem);
    SetNZFlag(mem);
}

void CPU::DEX()
{
    regs.X--;
    SetNZFlag(regs.X);
}

void CPU::DEY()
{
    regs.Y--;
    SetNZFlag(regs.Y);
}

void CPU::ASL()
{
    bool c;
    uint8_t operand;

    switch (instr.addressingMode) {
    case AM_ACC:
        c = NTHBIT(regs.A, 7);
        regs.A <<= 1;
        break;
    default:
        operand = ReadByte(instr.effectiveAddress);
        c = NTHBIT(operand, 7);
        operand <<= 1;
        WriteByte(instr.effectiveAddress, operand);
        break;
    }

    SetZNCFlag((instr.addressingMode == AM_ACC) ? regs.A : operand, c);
}

void CPU::LSR()
{
    bool c;
    uint8_t operand;

    switch (instr.addressingMode) {
    case AM_ACC:
    case AM_IMM:
        c = NTHBIT(regs.A, 0);
        regs.A >>= 1;
        break;
    default:
        operand = ReadByte(instr.effectiveAddress);
        c = NTHBIT(operand, 0);
        operand >>= 1;
        WriteByte(instr.effectiveAddress, operand);
        break;
    }

    SetZNCFlag((instr.addressingMode == AM_ACC || instr.addressingMode == AM_IMM) ? regs.A : operand, c);
    flags.N = 0;
}

void CPU::ROL()
{
    bool prevC = flags.C, currC;
    uint8_t operand;

    switch (instr.addressingMode) {
    case AM_ACC:
        currC = NTHBIT(regs.A, 7);
        regs.A = (regs.A << 1) | (uint8_t)prevC;
        break;
    default:
        operand = ReadByte(instr.effectiveAddress);
        currC = NTHBIT(operand, 7);
        operand = (operand << 1) | (uint8_t)prevC;
        WriteByte(instr.effectiveAddress, operand);
        break;
    }

    SetZNCFlag((instr.addressingMode == AM_ACC) ? regs.A : operand, currC);
}

void CPU::ROR()
{
    bool prevC = flags.C, currC;
    uint8_t operand;

    switch (instr.addressingMode) {
    case AM_ACC:
        currC = NTHBIT(regs.A, 0);
        regs.A = (regs.A >> 1) | ((uint8_t)prevC << 7);
        break;
    default:
        operand = ReadByte(instr.effectiveAddress);
        currC = NTHBIT(operand, 0);
        operand = (operand >> 1) | ((uint8_t)prevC << 7);
        WriteByte(instr.effectiveAddress, operand);
        break;
    }

    SetZNCFlag((instr.addressingMode == AM_ACC || instr.addressingMode == AM_IMM) ? regs.A : operand, currC);
}

void CPU::JMP()
{
    if (instr.addressingMode == AM_IND) {
        uint8_t lsb = ReadByte(U16(instr.operand[0], instr.operand[1]));
        uint8_t msb = ReadByte(U16(instr.operand[0], instr.operand[1] - ((instr.operand[0] == 0xff))) + 1);
        regs.PC = U16(lsb, msb);
    } else {
        regs.PC = instr.effectiveAddress;
    }
}

void CPU::JSR()
{
    PushWord(regs.PC - 1);
    regs.PC = instr.effectiveAddress;
}

void CPU::RTS()
{
    regs.PC = PopWord() + 1;
}

void CPU::BCC() 
{ 
    if (!flags.C) {
        regs.PC += (int8_t)instr.operand[0];
        instr.bonusCycles = (instr.pageCrossed) ? 2 : 1;
    }
}

void CPU::BCS() 
{ 
    if (flags.C) {
        regs.PC += (int8_t)instr.operand[0];
        instr.bonusCycles = (instr.pageCrossed) ? 2 : 1;
    }
}

void CPU::BEQ() 
{ 
    if (flags.Z) {
        regs.PC += (int8_t)instr.operand[0];
        instr.bonusCycles = (instr.pageCrossed) ? 2 : 1;
    }
}

void CPU::BMI() 
{ 
    if (flags.N) {
        regs.PC += (int8_t)instr.operand[0];
        instr.bonusCycles = (instr.pageCrossed) ? 2 : 1;
    }
}

void CPU::BNE() 
{ 
    if (!flags.Z) {
        regs.PC += (int8_t)instr.operand[0];
        instr.bonusCycles = (instr.pageCrossed) ? 2 : 1;
    }
}

void CPU::BPL() 
{ 
    if (!flags.N) {
        regs.PC += (int8_t)instr.operand[0];
        instr.bonusCycles = (instr.pageCrossed) ? 2 : 1;
    }
}

void CPU::BVC() 
{ 
    if (!flags.V) {
        regs.PC += (int8_t)instr.operand[0];
        instr.bonusCycles = (instr.pageCrossed) ? 2 : 1;
    }
}

void CPU::BVS() 
{ 
    if (flags.V) {
        regs.PC += (int8_t)instr.operand[0];
        instr.bonusCycles = (instr.pageCrossed) ? 2 : 1;
    }
}

void CPU::CLC() { flags.C = 0; }
void CPU::CLD() { flags.D = 0; }
void CPU::CLI() { flags.I = 0;}
void CPU::CLV() { flags.V = 0; }
void CPU::SEC() { flags.C = 1; }
void CPU::SED() { flags.D = 1; }
void CPU::SEI() { flags.I = 1; }

void CPU::BRK()
{
    uint8_t lsb = ReadByte(IRQ_START);
    // uint8_t lsb = ReadByte(IRQ_START) | 0x12;
    uint8_t msb = ReadByte(IRQ_END);

    PushWord(regs.PC + 1);
    PushByte(flags.P | (1U << 4));
    flags.I = 1;
    regs.PC = U16(lsb, msb);
}

void CPU::NOP()
{
    switch (instr.addressingMode) {
    case AM_ABSX:
        instr.bonusCycles = (instr.pageCrossed) ? 1 : 0;
        break;
    default:
        break;
    }
}

void CPU::RTI()
{
    uint8_t p = PopByte();

    /* N V 1 B D I Z C */
    flags.N = NTHBIT(p, 7);
    flags.Z = NTHBIT(p, 1);
    flags.C = NTHBIT(p, 0);
    flags.I = NTHBIT(p, 2);
    flags.D = NTHBIT(p, 3);
    flags.V = NTHBIT(p, 6);

    regs.PC = PopWord();
}

void CPU::TAS()
{
    printf("Illegal instruction - %02x\n", instr.opcode);
}

void CPU::JAM()
{
    printf("Illegal instruction - %02x\n", instr.opcode);
}

void CPU::SLO()
{
    ASL();
    ORA();
}

void CPU::ANC()
{
    AND();
    flags.C = NTHBIT(regs.A, 7);
}

void CPU::RLA()
{
    ROL();
    AND();
}

void CPU::SRE() 
{
    LSR();
    EOR();
}

void CPU::PHA()
{
    PushByte(regs.A);
}

void CPU::ALR()
{
    AND();
    LSR();
}

void CPU::RRA()
{
    ROR();
    ADC();
}

void CPU::ARR()
{
    AND();
    ROR();
}

void CPU::SAX()
{
    uint8_t result = regs.A & regs.X;
    WriteByte(instr.effectiveAddress, result);
}

void CPU::ANE()
{
    printf("Illegal instruction - %02x\n", instr.opcode);
}

void CPU::SHA()
{
    printf("Illegal instruction - %02x\n", instr.opcode);
}

void CPU::SHY()
{
    printf("Illegal instruction - %02x\n", instr.opcode);
}

void CPU::SHX()
{
    printf("Illegal instruction - %02x\n", instr.opcode);
}

void CPU::LAX()
{
    LDA();
    LDX();
}

void CPU::LXA()
{
    printf("Illegal instruction - %02x\n", instr.opcode);
}

void CPU::LAS()
{
    uint8_t operand = ReadByte(instr.effectiveAddress);

    instr.bonusCycles = (instr.pageCrossed) ? 1 : 0;
    operand &= regs.S;
    regs.A = regs.X = regs.S = operand;
    SetNZFlag(operand);
}

void CPU::DCP()
{
    DEC();
    CMP();
}

void CPU::SBX()
{
    uint8_t A = regs.A & regs.X;

    regs.X = A - instr.operand[0];
    SetZNCFlag(regs.X, A >= instr.operand[0]);
}

void CPU::ISC()
{
    INC();
    SBC();
}

void CPU::USBC()
{
    SBC();
    NOP();
}
#include "cpu.h"
#include <cstring>

/* CPU */

void CPU::SetNZFlag(uint8_t n)
{
    flags.N = NTHBIT(n, 7);
    flags.Z = !n;
}

void CPU::SetZNVFlag(uint8_t n)
{
    flags.Z = !n;
    flags.N = NTHBIT(n, 7);
    flags.V = NTHBIT(n, 6);
}

void CPU::SetZNCFlag(uint8_t n, bool c)
{
    flags.Z = !n;
    flags.N = NTHBIT(n, 7);
    flags.C = c;
}

void CPU::GetInstruction()
{
    uint16_t baseAddress;

    instr.opcode = ReadByte(regs.PC++);
    instr.addressingMode = opcodeList[instr.opcode].addrMode;
    switch (instr.addressingMode) {
    case AM_IMM:
        instr.operand[0] = ReadByte(regs.PC++);
        break;
    case AM_ZP:
        instr.operand[0] = ReadByte(regs.PC++);
        instr.effectiveAddress = instr.operand[0];
        break;
    case AM_ZPX:
        instr.operand[0] = ReadByte(regs.PC++);
        instr.effectiveAddress = (instr.operand[0] + regs.X) & 0x00ff;
        break;
    case AM_ZPY:
        instr.operand[0] = ReadByte(regs.PC++);
        instr.effectiveAddress = (instr.operand[0] + regs.Y) & 0x00ff;
        break;
    case AM_ABS:    
        instr.operand[0] = ReadByte(regs.PC++);
        instr.operand[1] = ReadByte(regs.PC++);
        instr.effectiveAddress = U16(instr.operand[0], instr.operand[1]);
        break;
    case AM_ABSX:
        instr.operand[0] = ReadByte(regs.PC++);
        instr.operand[1] = ReadByte(regs.PC++);
        instr.effectiveAddress = U16(instr.operand[0], instr.operand[1]) + regs.X;
        instr.pageCrossed = MSB(U16(instr.operand[0], instr.operand[1])) != MSB(instr.effectiveAddress);
        break;
    case AM_ABSY:
        instr.operand[0] = ReadByte(regs.PC++);
        instr.operand[1] = ReadByte(regs.PC++);
        instr.effectiveAddress = U16(instr.operand[0], instr.operand[1]) + regs.Y;
        instr.pageCrossed = MSB(U16(instr.operand[0], instr.operand[1])) != MSB(instr.effectiveAddress);
        break;
    case AM_XIND:
        instr.operand[0] = ReadByte(regs.PC++);
        baseAddress = ((uint16_t)instr.operand[0] + regs.X);
        instr.effectiveAddress = U16(ReadByte(baseAddress & 0x00ff), ReadByte((baseAddress + 1) & 0x00ff));
        break;
    case AM_INDY:
        instr.operand[0] = ReadByte(regs.PC++);
        baseAddress = instr.operand[0];
        instr.effectiveAddress = U16(ReadByte(baseAddress), ReadByte((baseAddress + 1) & 0x00ff)) + regs.Y; 
        instr.pageCrossed = MSB(U16(ReadByte(baseAddress), ReadByte((baseAddress + 1) & 0x00ff))) != MSB(instr.effectiveAddress);
        break;
    case AM_REL:
        instr.operand[0] = ReadByte(regs.PC++);
        instr.effectiveAddress = regs.PC + (int8_t)instr.operand[0];
        instr.pageCrossed = MSB(regs.PC) != MSB(instr.effectiveAddress);
        break;
    case AM_IND:
        instr.operand[0] = ReadByte(regs.PC++);
        instr.operand[1] = ReadByte(regs.PC++);
        baseAddress = U16(instr.operand[0], instr.operand[1]);
        instr.effectiveAddress = U16(ReadByte(baseAddress), ReadByte(baseAddress + 1));
        break;
    default:
        break;
    }
}

void CPU::HandleOpcode()
{
    (this->*opcodeList[instr.opcode].handler)();
}

int CPU::Step()
{
    GetInstruction();
    instr.bonusCycles = 0;
    HandleOpcode();
    return instr.bonusCycles + opcodeList[instr.opcode].baseCycle;
}

uint8_t CPU::ReadByte(uint16_t addr) 
{  
    return pMMU->cpuRead(addr);
}

uint16_t CPU::ReadWord(uint16_t addr)
{
    uint8_t lsb = ReadByte(addr);
    uint8_t msb = ReadByte(addr + 1);
    return U16(lsb, msb);
}

void CPU::WriteByte(uint16_t addr, uint8_t val)
{
    pMMU->cpuWrite(addr, val);
}

void CPU::PushByte(uint8_t val)
{
    WriteByte(STACK_START + regs.S, val);
    regs.S--; 
}

void CPU::PushWord(uint16_t val)
{
    PushByte(MSB(val));
    PushByte(LSB(val));
}

uint8_t CPU::PopByte()
{
    regs.S++;
    return ReadByte(STACK_START + regs.S);
}

uint16_t CPU::PopWord()
{
    uint8_t lsb = PopByte();
    uint8_t msb = PopByte();
    return U16(lsb, msb);
}

void CPU::WriteRAM(uint16_t addr, uint8_t val)
{
    pMMU->cpuWrite(addr, val);
}

uint8_t CPU::ReadRAM(uint16_t addr)
{
    return pMMU->cpuRead(addr);
}

void CPU::Init(MMU *_MMU)
{
    pMMU = _MMU;
}

/* constructor/destructor */

CPU::CPU()
{
    regs.A = regs.X = regs.Y = 0;
    regs.PC = 0xfffc;
    regs.S = 0xfd;
    flags.C = flags.Z = flags.D = flags.V = flags.N = 0;
    flags.I = 1;
}

CPU::~CPU()
{

}
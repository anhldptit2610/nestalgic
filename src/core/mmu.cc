#include "mmu.h"

#include <pstl/unseq_backend_simd.h>

void MMU::Init(PPU *_ppu, ROM *_rom, Controller *_controller)
{
    pPPU = _ppu;
    pROM = _rom;
    pController = _controller;
}

uint8_t MMU::ppuRegRead(uint16_t addr) { return pPPU->RegRead(addr); }

void MMU::ppuRegWrite(uint16_t addr, uint8_t val) { pPPU->RegWrite(addr, val); }

uint16_t MMU::cpuReadWord(uint16_t addr) { return U16(cpuReadByte(addr),cpuReadByte(addr + 1)); }

uint8_t MMU::cpuReadByte(uint16_t addr)
{
    switch (addr >> 12) {
    case 0:
    case 1:
        return cpuRAM[addr & 0x07ff];
    case 2:
    case 3:
        return ppuRegRead(addr & 0x2007);
    case 4:
        switch (addr & 0xff) {
        case 0x16:
        case 0x17:
            return pController->Read(addr);
        default:
            break;
        }
    default:
        if (IN_RANGE(addr, 0x8000, 0xffff))
            return pROM->prgROMRead(addr);
        break;
    }
    return 0xff;
}

void MMU::cpuWrite(uint16_t addr, uint8_t val)
{
    switch (addr >> 12) {
    case 0:
    case 1:
        cpuRAM[addr & 0x07ff] = val;
        break;
    case 2:
    case 3:
        if (IN_RANGE(addr, 0x3000, 0x3fff))
            printf("ppu reg write. Addr: %04x val: %02x\n", addr, val);
        ppuRegWrite(addr & 0x2007, val);
        break;
    case 4:
        switch (addr & 0xff) {
        case 0x16:
        case 0x17:
            pController->Write(addr, val);
            break;
        default:
            break;
        }
    default:
        if (IN_RANGE(addr, 0x8000, 0xffff))
            pROM->prgROMWrite(addr, val);
        break;
    }
}

MMU::MMU() {}
MMU::~MMU() {}
#include "mmu.h"


uint8_t MMU::cpuRead(uint16_t addr)
{
    switch (addr >> 12) {
    case 0:
    case 1:
        return cpuRAM[addr & 0x07ff];
    default:
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
    default:
        break;
    }
}

MMU::MMU() {}
MMU::~MMU() {}
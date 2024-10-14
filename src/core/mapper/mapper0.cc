#include "rom/rom.h"

uint8_t ROM::Mapper0Read(uint16_t addr)
{
    if (IN_RANGE(addr, 0x8000, 0xbfff)) {
        return prgROM[addr - 0x8000];
    } else if (IN_RANGE(addr, 0xc000, 0xffff) && header.prgROMSize == 32 * KiB) {
        return prgROM[addr - 0x8000];
    } else if (IN_RANGE(addr, 0xc000, 0xffff) && header.prgROMSize == 16 * KiB) {
        return prgROM[(addr & 0xbfff) - 0x8000];
    }
}

void ROM::Mapper0Write(uint16_t addr, uint8_t val)
{

}


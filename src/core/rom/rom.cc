#include "rom/rom.h"
#include <filesystem>

void ROM::PrintROMInfo()
{
    printf("Name: %s\n", name.c_str());
    printf("PRGROM size: %u\n", header.prgROMSize);
    printf("mapper: %d\n", header.mapper);
    printf("Mirroring: %s\n", (!header.mirror) ? "horizontal" : "vertical");
}

void ROM::CopyROM(std::ifstream& file, bool *quit)
{
    if (!*quit) {
        prgROM = new uint8_t[header.prgROMSize];
        if (!prgROM) {
            printf("PRGROM allocate failed\n");
            *quit = true;
        }
        chrROM = new uint8_t[header.chrROMSize];
        if (!chrROM) {
            printf("CHRROM allocate failed\n");
            *quit = true;
        }
    }
    file.read((char *)prgROM, header.prgROMSize);
    file.read((char *)chrROM, header.chrROMSize);
}

int ROM::ParseHeader(std::ifstream& file)
{
    file.read((char *)header.raw, 16);

    if (header.raw[0] != 0x4e || header.raw[1] != 0x45 ||
        header.raw[2] != 0x53 || header.raw[3] != 0x1a) {
        printf("This file is not a NES ROM file.\n");
        return -1;
    }

    header.prgROMSize = 16 * KiB * header.raw[4];
    header.chrROMSize = 8 * KiB * header.raw[5];
    header.mirror = (EMirroring)NTHBIT(header.raw[6], 0);
    header.mapper = (header.raw[6] >> 4) | (header.raw[7] & 0xf0);
    return 0;
}

uint8_t ROM::Read(uint16_t addr)
{
    if (header.mapper == 0)
        return Mapper0Read(addr);
    return 0xff;
}

void ROM::Write(uint16_t addr, uint8_t val)
{
}

/* constructor/destructor */

ROM::ROM(char *cartPath, bool *quit)
{
    std::ifstream myFile(cartPath);

    name = std::filesystem::path(cartPath).filename();
    *quit = (ParseHeader(myFile) == -1) ? true : false;
    if (!*quit)
        PrintROMInfo();
    CopyROM(myFile, quit);
    myFile.close();
}

ROM::~ROM()
{
    delete prgROM;
    delete chrROM;
}
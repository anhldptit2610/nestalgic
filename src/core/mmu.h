#ifndef MMU_H_
#define MMU_H_

#include "common.h"
#include "ppu.h"
#include "controller.h"
#include "rom.h"

class MMU {
private:
    uint8_t cpuRAM[CPU_RAM_SIZE];
    PPU *pPPU = nullptr;
    ROM *pROM = nullptr;
    Controller *pController = nullptr;
public:
    uint8_t cpuReadByte(uint16_t);
    uint16_t cpuReadWord(uint16_t);
    void cpuWrite(uint16_t, uint8_t);

    uint8_t ppuRegRead(uint16_t);
    void ppuRegWrite(uint16_t, uint8_t);

    void Init(PPU *, ROM *, Controller *);
    void RunOAMDMA(uint8_t *, uint16_t);

    /* constructor/destructor */
    MMU();
    ~MMU();
};

#endif
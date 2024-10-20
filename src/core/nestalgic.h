#ifndef NESTALGIC_H_
#define NESTALGIC_H_

#include "cpu/cpu.h"
#include "mmu.h"
#include "ppu.h"
#include "rom.h"
#include "graphic.h"
#include "logging.h"

class Nestalgic {
private:
    ROM rom;
    CPU cpu;
    PPU ppu;
    Controller controller;
    MMU mmu;
    Graphic graphic;
    Log log;

    int totalCycles;
    bool quit;
public:
    void RunAInstruction();
    void LogCurrentCPUState();
    bool Quit();
    void Init();
    void RunFrame();

    /* constructor/destructor */
    Nestalgic(char *);
    ~Nestalgic();
};

#endif
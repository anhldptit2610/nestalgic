#ifndef NESTALGIC_H_
#define NESTALGIC_H_

#include "cpu/cpu.h"
#include "mmu.h"
#include "rom.h"
#include "graphic.h"

class Nestalgic {
private:
    CPU cpu;
    MMU mmu;
    ROM rom;
    Graphic graphic;
    bool quit;
public:
    bool Quit();
    void Init();
    void RunFrame();

    /* constructor/destructor */
    Nestalgic(char *);
    ~Nestalgic();
};

#endif
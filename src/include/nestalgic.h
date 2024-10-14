#ifndef NESTALGIC_H_
#define NESTALGIC_H_

#include "cpu/cpu.h"
#include "mmu/mmu.h"
#include "rom/rom.h"

class Nestalgic {
private:
    CPU cpu;
    MMU mmu;
    ROM rom;
    bool quit;
public:
    bool Quit();
    void Init();

    /* constructor/destructor */
    Nestalgic(char *);
    ~Nestalgic();
};

#endif
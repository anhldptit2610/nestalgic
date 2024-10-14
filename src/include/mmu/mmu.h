#ifndef MMU_H_
#define MMU_H_

#include "../common.h"

class MMU {
private:
    uint8_t cpuRAM[CPU_RAM_SIZE];
public:
    uint8_t cpuRead(uint16_t addr);
    void cpuWrite(uint16_t addr, uint8_t val);

    /* constructor/destructor */
    MMU();
    ~MMU();
};

#endif
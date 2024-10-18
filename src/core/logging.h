#ifndef LOGGING_H_
#define LOGGING_H_

#include "common.h"
#include "cpu/cpu.h"

class Log {
private:
    FILE *fp = nullptr;
public:
    void LogCPUStateInNestest(int, int, int, uint8_t, const CPURegisters&, const CPUFlags&);

    /* constructor/destructor */
    Log();
    ~Log();
};

#endif
#include "logging.h"
#include <cstring>

void Log::LogCPUStateInNestest(int totalCycles, int scanLine, int ppuTicks, 
            uint8_t opcode, const CPURegisters& regs, const CPUFlags& flags)
{
#ifdef LOGGING
    char log[100];
    sprintf(log, "%04X  %02X A:%02X X:%02X Y:%02X P:%02X SP:%02X PPU:%3d,%3d CYC:%d\n", 
            regs.PC, opcode, regs.A, regs.X,
                regs.Y, flags.P, regs.S, scanLine, ppuTicks, totalCycles);
    fwrite(log, sizeof(char), strlen(log), fp);
#endif
}

/* constructor/destructor */
Log::Log()
{
    fp = fopen("log.txt", "w");
    if (!fp) {
        printf("Can't create log file\n");
    }
}

Log::~Log()
{
    if (fp != nullptr)
        fclose(fp);
}
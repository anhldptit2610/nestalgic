#include "nestalgic.h"


void Nestalgic::RunAInstruction()
{
    int cycles = 0;

    log.LogCPUStateInNestest(totalCycles, ppu.GetScanline(), ppu.GetTicks(),
                    cpu.GetOpcode(), cpu.GetCPURegister(), cpu.GetCPUFlags());
    cycles = cpu.Step();
    totalCycles += cycles;
    ppu.Step(cycles);
}

void Nestalgic::RunFrame()
{
    int cycles = 0;

    while (!ppu.IsFrameReady()) {
        log.LogCPUStateInNestest(totalCycles, ppu.GetScanline(), ppu.GetTicks(),
                        cpu.GetOpcode(), cpu.GetCPURegister(), cpu.GetCPUFlags());
        cycles = cpu.Step();
        ppu.Step(cycles);
        if (ppu.PullNMI())
            cpu.GenerateNMI();
    }
    ppu.SetFrameNotReady();
    ppu.UpdateTables();
    graphic.GetInput(&quit, controller.GetAllButtons());
    graphic.Draw(ppu.GetScreenFrameBuffer(), ppu.GetPTFrameBuffer());
}

void Nestalgic::Init()
{
#if NESTEST
    totalCycles = 7;
#else 
    totalCycles = 0;
#endif
    mmu.Init(&ppu, &rom, &controller);
    cpu.Init(&mmu);
    ppu.Init(&rom, rom.GetMirroringType());
}

bool Nestalgic::Quit()
{
    return quit;
}

/* debugging methods */

void Nestalgic::LogCurrentCPUState()
{
#ifdef LOGGING
    log.LogCPUStateInNestest(totalCycles, ppu.GetScanline(), ppu.GetTicks(),
            cpu.GetOpcode(), cpu.GetCPURegister(), cpu.GetCPUFlags());
#endif
}

Nestalgic::Nestalgic(char *romPath) : rom(romPath, &quit)
{

}

Nestalgic::~Nestalgic()
{

}
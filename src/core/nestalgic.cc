#include "nestalgic.h"

void Nestalgic::Init()
{
    cpu.Init(&mmu);
}

bool Nestalgic::Quit()
{
    return quit;
}

Nestalgic::Nestalgic(char *romPath) : rom(romPath, &quit)
{

}

Nestalgic::~Nestalgic()
{

}
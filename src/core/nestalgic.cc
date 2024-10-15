#include "nestalgic.h"

void Nestalgic::RunFrame()
{
    graphic.GetInput(&quit);
    graphic.Draw();     
}

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
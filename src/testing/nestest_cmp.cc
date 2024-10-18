#include "nestalgic.h"

int main(int argc, char *argv[])
{
    Nestalgic nestalgic(argv[1]);
    if (!nestalgic.Quit()) {
        nestalgic.Init();
        for (int i = 0; i < 8991; i++) {
            nestalgic.RunAInstruction();
        }
    }
    return 0;
}
#include "nestalgic.h" 

int main(int argc, char *argv[])
{
    Nestalgic nestalgic(argv[1]);

    if (!nestalgic.Quit()) {
        printf("ok\n");
    }
    return 0;
}
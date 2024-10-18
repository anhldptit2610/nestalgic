#include "nestalgic.h" 
// #include "graphic/graphic.h"

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

int main(int argc, char *argv[])
{
    Nestalgic nestalgic(argv[1]);

    if (!nestalgic.Quit()) {
        nestalgic.Init();
        while (!nestalgic.Quit()) {
            nestalgic.RunFrame();
        }
    }
    return 0;
}
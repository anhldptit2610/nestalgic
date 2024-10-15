#ifndef GRAPHIC_H_
#define GRAPHIC_H_

#include "ui.h"
#include "sdl.h"

class Graphic {
private:
    SDL sdl;
    UI ui;
public:
    void GetInput(bool *quit);
    void Draw();
    /* constructor, destructor */
    Graphic();
    ~Graphic();
};

#endif